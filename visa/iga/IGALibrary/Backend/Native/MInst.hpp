/*===================== begin_copyright_notice ==================================

Copyright (c) 2017 Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


======================= end_copyright_notice ==================================*/
#ifndef IGA_BACKEND_NATIVE_MINST_HPP
#define IGA_BACKEND_NATIVE_MINST_HPP

#include "Field.hpp"
#include "../../bits.hpp"

#include <cstdint>

namespace iga
{
    // machine instruction
    struct MInst {
        MInst() {
            qw0 = qw1 = 0;
        }

        union {
            struct {uint32_t dw0, dw1, dw2, dw3; };
            struct {uint32_t dws[4];};
            struct {uint64_t qw0, qw1;};
            struct {uint64_t qws[2];};
        };

        bool testBit(int off) const {
            return getBits(off, 1) != 0;
        }

        uint64_t getBits(int off, int len) const {
            return iga::getBits<uint64_t,2>(qws, off, len);
        }

        uint64_t getFragment(const Fragment &f) const {
            switch (f.kind) {
            case Fragment::Kind::ZERO_FILL:
            case Fragment::Kind::ZERO_WIRES:
                return 0;
            case Fragment::Kind::ENCODED:
                return getBits(f.offset, f.length);
            default:
                IGA_ASSERT_FALSE("unreachable");
                return (uint64_t)-1;
            }
        }

        // gets a fragmented field
        uint64_t getBits(const Fragment &fLo, const Fragment &fHi) const {
            uint64_t lo = getBits(fLo.offset, fLo.length);
            uint64_t hi = getBits(fHi.offset, fHi.length);
            return ((hi << fLo.length) | lo);
        }

        uint64_t getField(const Field &f) const {
            uint64_t bits = 0;
            //
            int off = 0;
            for (const Fragment &fr : f.fragments) {
                if (fr.kind == Fragment::Kind::INVALID)
                    break;
                auto frag = fr.isZeroFill() ?
                    0 : getBits(fr.offset, fr.length);
                bits |= frag << off;
                off += fr.length;
            }
            //
            return bits;
        }

        // gets a fragmented field from an array of fields
        // the fields are ordered low bit to high bit
        // we stop when we find a field with length 0
        template <int N>
        uint64_t getBits(const Fragment ff[N]) const {
            uint64_t bits = 0;

            int off = 0;
            for (const Fragment &fr : ff) {
                if (fr.isInvalid()) {
                    break;
                }
                auto frag =
                    fr.isZeroFill() ?
                        0 :
                        iga::getBits<uint64_t,N>(qws, fr.offset, fr.length);
                bits |= frag << off;
                off += fr.length;
            }

            return bits;
        }

        // returns false if field is too small to hold the value
        bool setField(const Field &f, uint64_t val) {
            for (const auto &fr : f.fragments) {
                if (fr.kind == Fragment::Kind::INVALID)
                    break;
                const auto fragValue = val & fr.getMask();
                switch (fr.kind) {
                case Fragment::Kind::ENCODED:
                    if (!setBits(fr.offset, fr.length, fragValue)) {
                        // this overflows the fragment
                        return false;
                    }
                    break;
                case Fragment::Kind::ZERO_WIRES:
                case Fragment::Kind::ZERO_FILL:
                    if (fragValue != 0) {
                        // an intrinsic fragment has non-zero bits
                        // e.g. ternary Dst.Subreg[3:0] is non-zero
                        return false;
                    }
                    break;
                default:
                    IGA_ASSERT_FALSE("unreachable");
                }
                val >>= fr.length;
            }
            return val == 0; // ensure no high bits left over
        }

        bool setFragment(const Fragment &fr, uint64_t val) {
            if (fr.isEncoded())
                return iga::setBits<uint64_t,2>(qws, fr.offset, fr.length, val);
            else
                return (fr.isZeroFill() && val == 0);
        }

        bool setBits(int off, int len, uint64_t val) {
            return iga::setBits<uint64_t,2>(qws, off, len, val);
        }

        // checks if a machine instruction is compacted
        // if compacted, the high 64b are undefined
        bool isCompact() const {return testBit(29);}
    };
    static_assert(sizeof(MInst) == 16, "MInst must be 16 bytes");
}

#endif // IGA_BACKEND_NATIVE_MINST_HPP
