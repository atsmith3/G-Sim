/*
 * Andrew Smith
 *
 * Read SRC Property:
 *  This is the frontend module of the graphicionado accelerator;
 *  it performs a sequential read on the value of the src vertex.
 *
 *  It has to interface with the DRAM memory to fetch the active
 *  property. Edges are stored in the Scratchpad.
 *
 */

#ifndef READSRCPROPERTY_H
#define READSRCPROPERTY_H

#include <iostream>
#include <vector>
#include <cstdint>

namespace SimObj {

class ReadSrcProperty : public module {

};

} // namespace SimObj

#endif
