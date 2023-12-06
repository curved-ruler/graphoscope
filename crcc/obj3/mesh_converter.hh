#pragma once

#include "mesh_ux.hh"
#include "mesh_ix.hh"

#include <string>


namespace cr {

mesh_ux* bm1_to_meshux (const std::string& filename);
mesh_ix* bm1_to_meshix (const std::string& filename);

mesh_ux* terr_to_meshux (const std::string& filename);

}
