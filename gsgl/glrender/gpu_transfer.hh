
#pragma once

#include "../gsgl_config.hh"
#include <crcc/crcc.hh>

namespace gsgl {

void TXTtoGPU  (const std::vector<float>& obj, cr::rrr_buffers& buf);
void MUXtoGPU  (const cr::mesh_ux& obj, cr::rrr_buffers& buf);
void MUXtoGPU  (const cr::mesh_ux& triobj, const cr::mesh_ux& linobj, cr::rrr_buffers& buf);
void updateGPU (const cr::mesh_ux& obj, cr::rrr_buffers& buf);
void updateGPU (const cr::mesh_ux& triobj, const cr::mesh_ux& linobj, cr::rrr_buffers& buf);
void clearGPU  (cr::rrr_buffers& buffers);

}
