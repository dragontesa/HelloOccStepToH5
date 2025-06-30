#include <H5Cpp.h> // Include the HDF5 C++ API header
#include <iostream>
#include <string>
#include <vector>
#include <numeric> // For std::iota (optional, for simple data generation)

// Define the name of the HDF5 file
const std::string FILE_NAME("my_scalar_data.hdf5");
const std::string DATASET_NAME("ScalarFaceTemperature");

int main() {
    // Disable auto-error reporting to handle errors gracefully with try-catch
    // H5::Exception::dontPrint();

    try {
        // Create a new HDF5 file. H5F_ACC_TRUNC truncates file if it already exists.
        H5::H5File file(FILE_NAME, H5F_ACC_TRUNC);
        std::cout << "Successfully created HDF5 file: " << FILE_NAME << std::endl;

        // --- 1. Define the Datatype ---

        // H5T_IEEE_F64LE means 64-bit little-endian float (double in C++)
        // Use H5::PredType::NATIVE_DOUBLE for C++ double type, HDF5 will map it to H5T_IEEE_F64LE on compatible systems.
        H5::FloatType scalar_datatype = H5::PredType::NATIVE_DOUBLE;
        std::cout << "Defined HDF5 64-bit float datatype (using NATIVE_DOUBLE)." << std::endl;

        // --- 2. Prepare the Data for Writing ---

        // Your sample data is 15 "slices", each with 6 rows, and each row has 2 values.
        // Total elements = 15 * 6 * 2 = 180 doubles.
        std::vector<double> cpp_data_source = {
            1.26502, 0.0,    1.27023, 0.09375,  1.28299, 0.1875,   1.30313, 0.276267,
            1.30434, 0.28125,  1.33489, 0.375,    // (0,0,0) to (0,5,0)
            1.34375, 0.397382, 1.3789, 0.46875,  1.38437, 0.478968, 1.425, 0.538483,
            1.44832, 0.5625,   1.46563, 0.580843, // (1,0,0) to (1,5,0)
            1.50625, 0.611988, 1.54688, 0.631806, 1.5875, 0.643678, 1.62812, 0.649621,
            1.66875, 0.650822, 1.70938, 0.647934, // (2,0,0) to (2,5,0)
            1.75, 0.641228,   1.79062, 0.63068,  1.83125, 0.616013, 1.26502, 0.0,
            1.27023, 0.09375,  1.28299, 0.1875,   // (3,0,0) to (3,5,0)
            1.30313, 0.276267, 1.30434, 0.28125,  1.33489, 0.375,    1.34375, 0.397382,
            1.3789, 0.46875,  1.38437, 0.478968, // (4,0,0) to (4,5,0)
            1.425, 0.538483,   1.44832, 0.5625,   1.46563, 0.580843, 1.50625, 0.611988,
            1.54688, 0.631806, 1.5875, 0.643678, // (5,0,0) to (5,5,0)
            1.62812, 0.649621, 1.66875, 0.650822, 1.70938, 0.647934, 1.75, 0.641228,
            1.79062, 0.63068,  1.83125, 0.616013, // (6,0,0) to (6,5,0)
            1.26502, 0.0,    1.27023, 0.09375,  1.28299, 0.1875,   1.30313, 0.276267,
            1.30434, 0.28125,  1.33489, 0.375,    // (7,0,0) to (7,5,0)
            1.34375, 0.397382, 1.3789, 0.46875,  1.38437, 0.478968, 1.425, 0.538483,
            1.44832, 0.5625,   1.46563, 0.580843, // (8,0,0) to (8,5,0)
            1.50625, 0.611988, 1.54688, 0.631806, 1.5875, 0.643678, 1.62812, 0.649621,
            1.66875, 0.650822, 1.70938, 0.647934, // (9,0,0) to (9,5,0)
            1.75, 0.641228,   1.79062, 0.63068,  1.83125, 0.616013, 1.26502, 0.0,
            1.27023, 0.09375,  1.28299, 0.1875,   // (10,0,0) to (10,5,0)
            1.30313, 0.276267, 1.30434, 0.28125,  1.33489, 0.375,    1.34375, 0.397382,
            1.3789, 0.46875,  1.38437, 0.478968, // (11,0,0) to (11,5,0)
            1.425, 0.538483,   1.44832, 0.5625,   1.46563, 0.580843, 1.50625, 0.611988,
            1.54688, 0.631806, 1.5875, 0.643678, // (12,0,0) to (12,5,0)
            1.62812, 0.649621, 1.66875, 0.650822, 1.70938, 0.647934, 1.75, 0.641228,
            1.79062, 0.63068,  1.83125, 0.616013, // (13,0,0) to (13,5,0)
            1.26502, 0.0,    1.27023, 0.09375,  1.28299, 0.1875,   1.30313, 0.276267,
            1.30434, 0.28125,  1.33489, 0.375     // (14,0,0) to (14,5,0)
        };

        // For verification, ensure the data size matches the dimensions
        const size_t expected_total_elements = 15 * 6 * 2;
        if (cpp_data_source.size() != expected_total_elements) {
            std::cerr << "Error: Prepared data size (" << cpp_data_source.size()
                      << ") does not match expected total elements for dataspace ("
                      << expected_total_elements << "). Aborting." << std::endl;
            return 1;
        }

        std::cout << "Prepared " << cpp_data_source.size() << " double data entries." << std::endl;

        // --- 3. Define the Dataspace ---

        // The dataset is 3-dimensional with (15, 6, 2) dimensions
        hsize_t dims[3] = {15, 6, 2};
        // The maxdims specify the maximum extent of each dimension.
        // H5S_UNLIMITED indicates that the dimension can be extended.
        // For dimensions with fixed size, use the same value as dims.
        hsize_t maxdims[3] = {15, H5S_UNLIMITED, 2};

        // Create a 3D dataspace
        H5::DataSpace dataspace(3, dims, maxdims);
        std::cout << "Defined dataspace with dimensions: ("
                  << dims[0] << ", " << dims[1] << ", " << dims[2] << ")" << std::endl;
        std::cout << "  Max dimensions: (" << maxdims[0] << ", "
                  << (maxdims[1] == H5S_UNLIMITED ? "H5S_UNLIMITED" : std::to_string(maxdims[1]))
                  << ", " << maxdims[2] << ")" << std::endl;


        // --- 4. Create and Write the Dataset ---

        H5::DataSet dataset = file.createDataSet(DATASET_NAME, scalar_datatype, dataspace);
        std::cout << "Created dataset '" << DATASET_NAME << "'." << std::endl;

        // Write the data
        dataset.write(cpp_data_source.data(), scalar_datatype);
        std::cout << "Data written to dataset '" << DATASET_NAME << "'." << std::endl;

        // --- (Optional) Read back and verify ---
        std::cout << "\n--- Verifying Data by Reading Back ---" << std::endl;
        std::vector<double> hdf5_read_data(expected_total_elements);
        dataset.read(hdf5_read_data.data(), scalar_datatype);

        // Print a few elements to verify
        std::cout << "First few elements read back:" << std::endl;
        for (size_t i = 0; i < std::min((size_t)12, hdf5_read_data.size()); i += 2) { // Print first 6 (value, coord) pairs
            std::cout << "  (" << hdf5_read_data[i] << ", " << hdf5_read_data[i+1] << ")" << std::endl;
        }
        std::cout << "Last few elements read back:" << std::endl;
        for (size_t i = hdf5_read_data.size() - 6; i < hdf5_read_data.size(); i += 2) { // Print last 3 (value, coord) pairs
            std::cout << "  (" << hdf5_read_data[i] << ", " << hdf5_read_data[i+1] << ")" << std::endl;
        }

        std::cout << "\nDataset '" << DATASET_NAME << "' successfully created and verified." << std::endl;

    } catch (const H5::Exception& err) {
        std::cerr << "HDF5 Error: " << err.get<char>() << std::endl;
        return 1;
    }

    return 0;
}
