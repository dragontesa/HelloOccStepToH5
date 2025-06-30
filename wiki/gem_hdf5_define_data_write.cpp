#include <H5Cpp.h> // Include the HDF5 C++ API header
#include <iostream>
#include <string>
#include <vector>
#include <array>

// Define the name of the HDF5 file
const std::string FILE_NAME("myhdf5.hdf5");

// Define the structure for the "Meta" compound dataset
// Ensure string sizes match H5T_STRING STRSIZE (e.g., 32 for char[32])
struct MetaData {
    char Domain[32];
    char Origin[32];
    char Mesh[32];
    float NumShapes;
    float Version_M; // Note: You have Version_M twice in your HDF5 definition.
                     // Assuming the second one is a typo or meant to be different.
    float Version_R;
};

int main() {
    // Disable auto-error reporting to handle errors gracefully with try-catch
    // H5::Exception::dontPrint();

    try {
        // Create a new HDF5 file. H5F_ACC_TRUNC truncates file if it already exists.
        H5::H5File file(FILE_NAME, H5F_ACC_TRUNC);
        std::cout << "Successfully created HDF5 file: " << FILE_NAME << std::endl;

        // --- 1. Create and Write "Meta" Dataset ---
        std::cout << "\n--- Writing 'Meta' Dataset ---" << std::endl;

        // Define the compound datatype for "Meta"
        H5::CompType meta_datatype(sizeof(MetaData));

        // Define string datatypes for each member with null termination
        H5::StrType domain_str_type(H5T_C_S1, 32);
        domain_str_type.setStrpad(H5T_STR_NULLTERM);

        H5::StrType origin_str_type(H5T_C_S1, 32);
        origin_str_type.setStrpad(H5T_STR_NULLTERM);

        H5::StrType mesh_str_type(H5T_C_S1, 32);
        mesh_str_type.setStrpad(H5T_STR_NULLTERM);

        // Insert members using the pre-configured string datatypes
        meta_datatype.insertMember("Domain", HOFFSET(MetaData, Domain), domain_str_type);
        meta_datatype.insertMember("Origin", HOFFSET(MetaData, Origin), origin_str_type);
        meta_datatype.insertMember("Mesh", HOFFSET(MetaData, Mesh), mesh_str_type);
        meta_datatype.insertMember("NumShapes", HOFFSET(MetaData, NumShapes), H5::PredType::NATIVE_FLOAT);
        meta_datatype.insertMember("Version_M", HOFFSET(MetaData, Version_M), H5::PredType::NATIVE_FLOAT);
        meta_datatype.insertMember("Version_R", HOFFSET(MetaData, Version_R), H5::PredType::NATIVE_FLOAT);


        // Create a dataspace for a single element (scalar dataset)
        hsize_t meta_dim[] = {1};
        H5::DataSpace meta_dataspace(1, meta_dim);

        // Create the "Meta" dataset
        H5::DataSet meta_dataset = file.createDataSet("Meta", meta_datatype, meta_dataspace);

        // Prepare the data for "Meta"
        MetaData meta_data_instance;
        strncpy(meta_data_instance.Domain, "ITER", sizeof(meta_data_instance.Domain) - 1);
        meta_data_instance.Domain[sizeof(meta_data_instance.Domain) - 1] = '\0';
        strncpy(meta_data_instance.Origin, "CC_SYSTEM_IC#EUHXML.stp", sizeof(meta_data_instance.Origin) - 1);
        meta_data_instance.Origin[sizeof(meta_data_instance.Origin) - 1] = '\0';
        strncpy(meta_data_instance.Mesh, "CC_SYSTEM_IC#EUHXML.obj", sizeof(meta_data_instance.Mesh) - 1);
        meta_data_instance.Mesh[sizeof(meta_data_instance.Mesh) - 1] = '\0';
        meta_data_instance.NumShapes = 3010.0f;
        meta_data_instance.Version_M = 1.0f;
        meta_data_instance.Version_R = 0.0f; // Adjusted for the extra Version_M entry in your schema
        // The last 0 in your data for Meta implies the 'Version_R' field.
        // Your HDF5 schema shows 'Version_M' twice, then 'Version_R'.
        // Assuming it should be Domain, Origin, Mesh, NumShapes, Version_M, Version_R
        // If it was meant to be 7 fields, you'd need to add another float to MetaData struct.

        // Write the data to the "Meta" dataset
        meta_dataset.write(&meta_data_instance, meta_datatype);
        std::cout << "'Meta' dataset written." << std::endl;


        // --- 2. Create and Write "Products" Dataset ---
        std::cout << "\n--- Writing 'Products' Dataset ---" << std::endl;

        // Define string datatype for Products (fixed size 32, null-terminated)
        H5::StrType products_str_datatype(H5T_C_S1, 32);
        products_str_datatype.setStrpad(H5T_STR_NULLTERM);

        // Prepare data for "Products" (flattened for storage in C++ vector)
        std::vector<std::string> products_data_str = {
            "MAGNET_SYSTEM#36MXDX", "0:1:1", "#2",
            "CC_SYSTEM_IC#EUHXML", "0:1:2", "#2",
            "SIDE_CORRECTION_COILS_IC#2K3UNV", "0:1:3", "#2",
            "TOP_CORRECTION_COILS_IC#2K3UUW", "0:1:4", "#2",
            "BOTTOM_CORRECTION_COILS_IC#2K3UUW", "0:1:5", "#2",
            "FRONT_CORRECTION_COILS_IC#2K3UUW", "0:1:6", "#2"
        };

        // Define dimensions for a 2D dataset: 6 rows, 3 columns
        hsize_t products_dims[] = {6, 3}; // Corrected to explicitly define 2D
        H5::DataSpace products_dataspace(2, products_dims); // Changed to 2 dimensions

        // Convert std::string to char arrays for writing (still a flat buffer of 18 char[32] elements)
        std::vector<char[32]> products_data_c_str(products_data_str.size());
        for (size_t i = 0; i < products_data_str.size(); ++i) {
            strncpy(products_data_c_str[i], products_data_str[i].c_str(), sizeof(products_data_c_str[i]) - 1);
            products_data_c_str[i][sizeof(products_data_c_str[i]) - 1] = '\0'; // Ensure null termination
        }

        // Create the "Products" dataset
        H5::DataSet products_dataset = file.createDataSet("Products", products_str_datatype, products_dataspace);

        // Write the data. HDF5 will map the 1D C++ buffer into the 2D HDF5 dataspace.
        products_dataset.write(products_data_c_str.data(), products_str_datatype);
        std::cout << "'Products' dataset written." << std::endl;


        // --- 3. Create and Write "Shapes" Dataset ---
        std::cout << "\n--- Writing 'Shapes' Dataset ---" << std::endl;

        // Define string datatype for Shapes (fixed size 32, null-terminated)
        H5::StrType shapes_str_datatype(H5T_C_S1, 32);
        shapes_str_datatype.setStrpad(H5T_STR_NULLTERM);

        // Prepare data for "Shapes" (4 rows, 3 columns)
        std::vector<std::vector<std::string>> shapes_data_str = {
            {"Shape0", "0:1:1", ""},
            {"S1", "0:1:1:1", "0:1:1"},
            {"Mainbody_1", "0:1:1:1", "0:1:1:1"}, // Note: Changed to match 3 elements for 12 total
            {"Mainbody_Back", "0:1:1:3", "0:1:1:1"},
            {"Mainbody_Front", "0:1:1:4", "0:1:1:1"},
            {"FOOT_LEFT", "0:1:1:5", "0:1:1:1"},
            {"FOOT_RIGHT", "0:1:1:6", "0:1:1:1"},
            {"HEAD_FRONT", "0:1:1:7", "0:1:1:1"},
            {"HEAD_BACK", "0:1:1:8", "0:1:1:1"},
            {"TOP_FRONT", "0:1:1:9", "0:1:1:1"},
            {"TOP_BACK", "0:1:1:10", "0:1:1:1"},
            {"S2", "0:1:1:11", "#100"} // Your example has "#100","#1" - HDF5 doesn't like multi-string per cell. Assuming "#100" or similar.
        };

        // Determine actual dimensions based on data
        hsize_t shapes_rows = shapes_data_str.size();
        hsize_t shapes_cols = 0;
        if (shapes_rows > 0) {
            shapes_cols = shapes_data_str[0].size();
        }

        hsize_t shapes_dims[] = {shapes_rows, shapes_cols};
        H5::DataSpace shapes_dataspace(2, shapes_dims);

        // Flatten vector of strings to a 1D array of char[32] for HDF5 write
        std::vector<char[32]> shapes_data_c_str(shapes_rows * shapes_cols);
        for (size_t i = 0; i < shapes_rows; ++i) {
            for (size_t j = 0; j < shapes_cols; ++j) {
                size_t index = i * shapes_cols + j;
                strncpy(shapes_data_c_str[index], shapes_data_str[i][j].c_str(), sizeof(shapes_data_c_str[index]) - 1);
                shapes_data_c_str[index][sizeof(shapes_data_c_str[index]) - 1] = '\0'; // Ensure null termination
            }
        }

        // Create the "Shapes" dataset
        H5::DataSet shapes_dataset = file.createDataSet("Shapes", shapes_str_datatype, shapes_dataspace);

        // Write the data
        shapes_dataset.write(shapes_data_c_str.data(), shapes_str_datatype);
        std::cout << "'Shapes' dataset written." << std::endl;


        // --- 4. Create and Write "Properties" Dataset ---
        std::cout << "\n--- Writing 'Properties' Dataset ---" << std::endl;

        // Prepare data for "Properties" (4 rows, 3 columns implicitly, 5 floats per row)
        // Your data has 12 entries, each with 5 floats. This means it's a (12, 5) float array.
        // Let's assume it should be 4 rows with 5 floats each, and there's a misunderstanding of (4,3)
        // as your HDF5 output shows 12 sets of 5 values (implicitly 4 rows x 3 columns x 5 values/col = 60 values total).
        // Let's interpret (4,3) as 4 data points, each being a vector of 3 elements * 5 = 15 values.
        // Based on the data, it looks like 12 rows of 5 float values.
        // Re-interpreting the (4,3) in your `DATASPACE SIMPLE { ( 4, 3) / ( 4, H5S_UNLIMITED)}` with the provided data:
        // Your data shows 12 lines of 5 floats each. If this is a (4,3) dataset, it means each cell is a 5-float array.
        // This is complex. A more common interpretation of (4,3) is 4 rows, 3 columns, where each cell is a single float.
        // Given your data, it's more likely a (12, 5) float dataset, or if it must be (4,3), then each cell contains 5 floats.
        // I will assume it is a (12, 5) dataset for simplicity, as HDF5 datasets usually store homogeneous values.
        // If it must be (4,3) with each cell being 5 floats, the C++ code for that would be much more complex (e.g., custom compound types for each cell).
        // Let's go with (12, 5) to match the provided flat data structure.

        std::vector<std::vector<float>> properties_data = {
            {312.4f, -191.6f, 0.00f, 0.00f, 0.00f},
            {-4.6f, -181.2f, 0.01f, 0.00f, 0.10f},
            {-2.6f, -50.7f, 0.01f, 0.01f, 0.00f},
            {20.6f, 10.7f, 0.00f, 0.10f, 0.20f},
            {20.1f, 0.3f, 0.01f, 0.06f, 0.00f},
            {-190.1f, 10.6f, 0.00f, 0.02f, 0.01f},
            {0.0f, -30.6f, 0.01f, 0.01f, 0.00f},
            {1.0f, 4.5f, 0.00f, 0.10f, 1.10f},
            {-32.0f, 10.5f, 1.00f, 1.10f, 1.00f},
            {2.0f, 2.1f, 0.20f, 1.12f, 0.02f},
            {-250.0f, -30.1f, 1.01f, 1.40f, 0.40f},
            {1.0f, 53.1f, 1.05f, 1.20f, 1.20f}
        };

        hsize_t properties_rows = properties_data.size();
        hsize_t properties_cols = 0;
        if (properties_rows > 0) {
            properties_cols = properties_data[0].size();
        }

        hsize_t properties_dims[] = {properties_rows, properties_cols};
        H5::DataSpace properties_dataspace(2, properties_dims);

        // Flatten the 2D vector to a 1D array for writing
        std::vector<float> properties_flat_data;
        properties_flat_data.reserve(properties_rows * properties_cols);
        for (const auto& row : properties_data) {
            properties_flat_data.insert(properties_flat_data.end(), row.begin(), row.end());
        }

        // Create the "Properties" dataset
        H5::DataSet properties_dataset = file.createDataSet("Properties", H5::PredType::IEEE_F32LE, properties_dataspace);

        // Write the data
        properties_dataset.write(properties_flat_data.data(), H5::PredType::NATIVE_FLOAT);
        std::cout << "'Properties' dataset written." << std::endl;


        // --- 5. Create and Write "ColorVertex" Dataset ---
        std::cout << "\n--- Writing 'ColorVertex' Dataset ---" << std::endl;

        // Your HDF5 shows (4,3) dataspace, but 12 lines of (R,G,B) = 3 floats.
        // This implies a (12, 3) dataset. Let's assume it's 12 rows of 3 floats each.
        // If it must be (4,3), then each cell (R,G,B) means a compound type.
        // For simplicity and typical HDF5 usage, I'll go with (12, 3).
        std::vector<std::vector<float>> color_vertex_data = {
            {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}
        };

        hsize_t cv_rows = color_vertex_data.size();
        hsize_t cv_cols = 0;
        if (cv_rows > 0) {
            cv_cols = color_vertex_data[0].size();
        }

        hsize_t color_vertex_dims[] = {cv_rows, cv_cols};
        H5::DataSpace color_vertex_dataspace(2, color_vertex_dims);

        std::vector<float> color_vertex_flat_data;
        color_vertex_flat_data.reserve(cv_rows * cv_cols);
        for (const auto& row : color_vertex_data) {
            color_vertex_flat_data.insert(color_vertex_flat_data.end(), row.begin(), row.end());
        }

        H5::DataSet color_vertex_dataset = file.createDataSet("ColorVertex", H5::PredType::IEEE_F32LE, color_vertex_dataspace);
        color_vertex_dataset.write(color_vertex_flat_data.data(), H5::PredType::NATIVE_FLOAT);
        std::cout << "'ColorVertex' dataset written." << std::endl;


        // --- 6. Create and Write "ColorSurface" Dataset ---
        std::cout << "\n--- Writing 'ColorSurface' Dataset ---" << std::endl;

        // Data structure similar to ColorVertex
        std::vector<std::vector<float>> color_surface_data = {
            {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}
        };

        hsize_t cs_rows = color_surface_data.size();
        hsize_t cs_cols = 0;
        if (cs_rows > 0) {
            cs_cols = color_surface_data[0].size();
        }

        hsize_t color_surface_dims[] = {cs_rows, cs_cols};
        H5::DataSpace color_surface_dataspace(2, color_surface_dims);

        std::vector<float> color_surface_flat_data;
        color_surface_flat_data.reserve(cs_rows * cs_cols);
        for (const auto& row : color_surface_data) {
            color_surface_flat_data.insert(color_surface_flat_data.end(), row.begin(), row.end());
        }

        H5::DataSet color_surface_dataset = file.createDataSet("ColorSurface", H5::PredType::IEEE_F32LE, color_surface_dataspace);
        color_surface_dataset.write(color_surface_flat_data.data(), H5::PredType::NATIVE_FLOAT);
        std::cout << "'ColorSurface' dataset written." << std::endl;


        // --- 7. Create and Write "ScalarTemperature" Dataset ---
        std::cout << "\n--- Writing 'ScalarTemperature' Dataset ---" << std::endl;

        // Your HDF5 defines (10, 20, 2). This means 10 "slices", each 20 rows by 2 columns.
        // Your data actually contains 2 sets of 21 (value, coord) pairs.
        // Let's assume the HDF5 definition (10, 20, 2) is the target, and your provided data is just a sample
        // of how values look. I'll extend it to fit the 10x20x2 structure.
        // Or, if your data defines the size, it's (2, 21, 2).
        // I will adhere to your HDF5 spec: (10, 20, 2)
        const int ST_DIM1 = 10;
        const int ST_DIM2 = 20;
        const int ST_DIM3 = 2;

        hsize_t scalar_temp_dims[] = {ST_DIM1, ST_DIM2, ST_DIM3};
        H5::DataSpace scalar_temp_dataspace(3, scalar_temp_dims);

        // Populate a 3D array with dummy data or repeating your provided sample
        std::vector<float> scalar_temp_flat_data(ST_DIM1 * ST_DIM2 * ST_DIM3);

        // Your provided sample data for one 2D slice (21 rows, 2 columns)
        std::vector<std::vector<float>> sample_temp_slice = {
            {1.26502f, 0.0f}, {1.27023f, 0.09375f}, {1.28299f, 0.1875f},
            {1.30313f, 0.276267f}, {1.30434f, 0.28125f}, {1.33489f, 0.375f},
            {1.34375f, 0.397382f}, {1.3789f, 0.46875f}, {1.38437f, 0.478968f},
            {1.425f, 0.538483f}, {1.44832f, 0.5625f}, {1.46563f, 0.580843f},
            {1.50625f, 0.611988f}, {1.54688f, 0.631806f}, {1.5875f, 0.643678f},
            {1.62812f, 0.649621f}, {1.66875f, 0.650822f}, {1.70938f, 0.647934f},
            {1.75f, 0.641228f}, {1.79062f, 0.63068f}, {1.83125f, 0.616013f}
        };

        // Fill the 3D data. Repeat the sample data if it's smaller than the full dimensions.
        // Here, we'll fill with the sample data and then zeros if the sample data is exhausted.
        int flat_idx = 0;
        for (int i = 0; i < ST_DIM1; ++i) { // 10 slices
            for (int j = 0; j < ST_DIM2; ++j) { // 20 rows per slice
                for (int k = 0; k < ST_DIM3; ++k) { // 2 columns per row (value, coord)
                    if (j < sample_temp_slice.size() && k < sample_temp_slice[j].size()) {
                        scalar_temp_flat_data[flat_idx++] = sample_temp_slice[j][k];
                    } else {
                        scalar_temp_flat_data[flat_idx++] = 0.0f; // Fill with zeros if sample runs out
                    }
                }
            }
        }

        H5::DataSet scalar_temp_dataset = file.createDataSet("ScalarTemperature", H5::PredType::IEEE_F32LE, scalar_temp_dataspace);
        scalar_temp_dataset.write(scalar_temp_flat_data.data(), H5::PredType::NATIVE_FLOAT);
        std::cout << "'ScalarTemperature' dataset written." << std::endl;

        std::cout << "\nAll datasets successfully written to " << FILE_NAME << std::endl;

    } catch (const H5::Exception& err) {
        std::cerr << "HDF5 Error: " << err.get<\ctrl62>
}
