#include <H5Cpp.h>
#include <string>

using namespace H5;

// Define the record structure
struct CoilRecord {
    const char* name;
    const char* address;
    const char* id;
};

int main() {
    // Sample data
    CoilRecord records[6] = {
        {"MAGNET_SYSTEM#36MXDX", "0:1:1", "#2"},
        {"CC_SYSTEM_IC#EUHXML", "0:1:2", "#2"},
        {"SIDE_CORRECTION_COILS_IC#2K3UNV", "0:1:3", "#2"},
        {"TOP_CORRECTION_COILS_IC#2K3UUW", "0:1:4", "#2"},
        {"BOTTOM_CORRECTION_COILS_IC#2K3UUW", "0:1:5", "#2"},
        {"FRONT_CORRECTION_COILS_IC#2K3UUW", "0:1:6", "#2"}
    };

    // Open or create HDF5 file
    H5File file("coils.h5", H5F_ACC_TRUNC);

    // Create a variable-length string type
    StrType strType(PredType::C_S1, H5T_VARIABLE);

    // Create a compound datatype
    CompType compType(sizeof(CoilRecord));
    compType.insertMember("name", HOFFSET(CoilRecord, name), strType);
    compType.insertMember("address", HOFFSET(CoilRecord, address), strType);
    compType.insertMember("id", HOFFSET(CoilRecord, id), strType);

    // Define a simple dataspace: 1D array of 6 records
    hsize_t dims[1] = {6};
    DataSpace space(1, dims);

    // Create and write dataset
    DataSet dataset = file.createDataSet("coil_metadata", compType, space);
    dataset.write(records, compType);

    file.close();
    return 0;
}
