#include <H5Cpp.h> // Include the HDF5 C++ API header
#include <iostream>
#include <string>
#include <vector>
#include <numeric> // For std::iota (optional, for simple data generation)

// Define the name of the HDF5 file
const std::string FILE_NAME("my_vlen_data.hdf5");
const std::string DATASET_NAME("vtx_idx");

int main() {
    // Disable auto-error reporting to handle errors gracefully with try-catch
    // H5::Exception::dontPrint();

    try {
        // Create a new HDF5 file. H5F_ACC_TRUNC truncates file if it already exists.
        H5::H5File file(FILE_NAME, H5F_ACC_TRUNC);
        std::cout << "Successfully created HDF5 file: " << FILE_NAME << std::endl;

        // --- 1. Define the Variable-Length Integer Datatype ---

        // Step 1.1: Define the base type for the elements within the VLEN array
        // H5T_STD_I32LE means 32-bit little-endian integer.
        // H5::PredType::NATIVE_INT is usually compatible with H5T_STD_I32LE on most systems,
        // but for strict adherence, you could use H5::PredType::STD_I32LE.
        H5::IntType base_int_type = H5::PredType::STD_I32LE;

        // Step 1.2: Create the Variable-Length type based on the integer type
        H5::VarLenType vlen_int_datatype(base_int_type);
        std::cout << "Defined HDF5 variable-length integer datatype." << std::endl;

        // --- 2. Prepare the Data for Writing ---

        // Your sample data indicates 14 variable-length elements.
        // Each element is a sequence of integers (like a tuple).
        std::vector<std::vector<int>> cpp_data_source = {
            {1, 11, 12}, {11, 1, 0}, {44540, 1, 12}, {44551, 44540, 12},
            {44550, 44539, 44551}, {44540, 44551, 44539},
            {44550, 13, 44539}, {3, 44539, 13}, {24, 8, 23}, {8, 24, 2},
            {44550, 13, 44539}, {3, 44539, 13}, {24, 8, 23}, {8, 24, 2}
        };

        // This is the C-style array of hvl_t structs that HDF5 expects for writing
        std::vector<hvl_t> hdf5_write_data(cpp_data_source.size());

        // Populate the hvl_t structs
        for (size_t i = 0; i < cpp_data_source.size(); ++i) {
            hdf5_write_data[i].len = cpp_data_source[i].size();
            // Allocate memory for the actual data within the hvl_t struct
            // It's crucial that this memory is allocated using C++ new[],
            // as HDF5 library expects to free it later using its own reclaim function.
            int* data_ptr = new int[cpp_data_source[i].size()];
            std::copy(cpp_data_source[i].begin(), cpp_data_source[i].end(), data_ptr);
            hdf5_write_data[i].p = data_ptr;
        }
        std::cout << "Prepared " << cpp_data_source.size() << " variable-length data entries." << std::endl;

        // --- 3. Define the Dataspace ---

        // The dataset is 1-dimensional with 14 elements (each element is a VLEN array)
        hsize_t dims[] = {cpp_data_source.size()}; // 14
        H5::DataSpace dataspace(1, dims); // 1 dimension, size 14
        std::cout << "Defined dataspace with dimensions: (" << dims[0] << ")" << std::endl;

        // --- 4. Create and Write the Dataset ---

        H5::DataSet dataset = file.createDataSet(DATASET_NAME, vlen_int_datatype, dataspace);
        std::cout << "Created dataset '" << DATASET_NAME << "'." << std::endl;

        // Write the data
        dataset.write(hdf5_write_data.data(), vlen_int_datatype);
        std::cout << "Data written to dataset '" << DATASET_NAME << "'." << std::endl;

        // --- 5. Reclaim memory for written HDF5 variable-length data ---
        // This is extremely important for HDF5 variable-length types.
        // HDF5 allocates memory internally when preparing the data for writing.
        // You must call H5Dvlen_reclaim to free the `p` pointers within the hvl_t structs
        // (the memory allocated by `new int[]`).
        H5D_vlen_reclaim(vlen_int_datatype.getId(), dataspace.getId(), H5P_DEFAULT, hdf5_write_data.data());
        std::cout << "Reclaimed memory for written VLEN data." << std::endl;

        // --- (Optional) Read back and verify ---
        std::cout << "\n--- Verifying Data by Reading Back ---" << std::endl;
        std::vector<hvl_t> hdf5_read_data(cpp_data_source.size());
        dataset.read(hdf5_read_data.data(), vlen_int_datatype);

        for (size_t i = 0; i < hdf5_read_data.size(); ++i) {
            std::cout << "  Read element " << i << " (length " << hdf5_read_data[i].len << "): (";
            for (size_t j = 0; j < hdf5_read_data[i].len; ++j) {
                std::cout << static_cast<int*>(hdf5_read_data[i].p)[j];
                if (j < hdf5_read_data[i].len - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << ")" << std::endl;
        }

        // --- Reclaim memory for read HDF5 variable-length data ---
        // Must also reclaim memory after reading VLEN data.
        H5D_vlen_reclaim(vlen_int_datatype.getId(), dataspace.getId(), H5P_DEFAULT, hdf5_read_data.data());
        std::cout << "Reclaimed memory for read VLEN data." << std::endl;

        std::cout << "\nDataset '" << DATASET_NAME << "' successfully created and verified." << std::endl;

    } catch (const H5::Exception& err) {
        std::cerr << "HDF5 Error: " << err.get<char>() << std::endl;
        return 1;
    }

    return 0;
}
