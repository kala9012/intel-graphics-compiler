/*
 * !!! DO NOT EDIT THIS FILE !!!
 *
 * This file was automagically crafted by GED's model parser.
 */


#ifndef GED_MODEL_GEN8__H
#define GED_MODEL_GEN8__H

#include "common/ged_ins_decoding_table.h"
#include "common/ged_compact_mapping_table.h"

namespace GEN8
{

/*!
 * This table maps every possible opcode value (even for non-existing opcodes) to its respective top level decoding, encoding
 * restrictions and mapping tables (where applicable). Tables that are not supported in this model (either no compaction, or opcodes
 * which are not supported at all) are mapped to NULL pointers.
 */
extern OpcodeTables Opcodes[128];

#if GED_DISASSEMBLY

/*!
 * Get the top level disassembly table for the given instruction.
 *
 * @param[in]       opcode The instruction's opcode.
 *
 * @return      The requested disassembly table if the instruction is supported, NULL otherwise.
 */
extern const ged_disassembly_table_t GetDisassemblyTable(/* GED_OPCODE */ uint32_t opcode);
#endif // GED_DISASSEMBLY
} // namespace GEN8
#endif // GED_MODEL_GEN8__H
