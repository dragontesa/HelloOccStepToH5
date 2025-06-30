/**
 * This is an excellent and fundamental question about HDF5 file operations!
Does HDF5 overwrite the previous dataset or get appended to it when you write a dataset with the same name?
The answer depends on two main things:
    Whether you are creating a new dataset with an existing name, or writing data to an existing dataset.
    The file access flags you use when opening or creating the HDF5 file.
    The specific write operation you perform (full dataset write vs. hyperslab/selection write).

Let's break it down:
1. Creating a New Dataset with an Existing Name
If you try to create a new dataset using file.createDataSet("MyDatasetName", ...) and a dataset with "MyDatasetName" already exists in the HDF5 file, what happens depends on how you opened or created the H5::H5File object:
    H5F_ACC_TRUNC (Truncate):
        Behavior: If you open the file with H5F_ACC_TRUNC (e.g., H5::H5File file(FILE_NAME, H5F_ACC_TRUNC);), and the file already exists, HDF5 will truncate the entire file (delete all its contents) before creating it. This means any datasets, groups, or attributes that were in the file are gone. Then, your new createDataSet call will succeed because it's effectively creating a dataset in a brand-new, empty file.
        Effect on Dataset: The dataset with the "same name" is effectively overwritten because the file itself was reset.
    H5F_ACC_EXCL (Exclusive):
        Behavior: If you create a file with H5F_ACC_EXCL (e.g., H5::H5File file(FILE_NAME, H5F_ACC_EXCL);), and the file already exists, the H5::H5File constructor will fail. You won't even get a file handle.
        Effect on Dataset: No new dataset is created, and the existing file/dataset is untouched.
    H5F_ACC_RDWR (Read-Write - for existing files):
        Behavior: If you open an existing file with H5F_ACC_RDWR (e.g., H5::H5File file(FILE_NAME, H5F_ACC_RDWR);), and then try to call file.createDataSet("MyDatasetName", ...) where "MyDatasetName" already exists within that file, the createDataSet call will fail with an error (an H5::Exception). HDF5 will not overwrite an existing dataset when attempting to create a new one with the same name in an already-opened file.
        Effect on Dataset: The existing dataset remains as is, and no new dataset is created.

In summary for creating a new dataset: You cannot create a new dataset with the same name as an existing one within the same file unless you truncated the file first (H5F_ACC_TRUNC), which effectively deletes everything.

2. Writing Data to an Existing Dataset
If you want to modify the data within an already existing dataset, you need to first open that dataset and then use its write() method.
    Opening the file: You must open the HDF5 file in read-write mode: H5::H5File file(FILE_NAME, H5F_ACC_RDWR);
    Opening the dataset: H5::DataSet dataset = file.openDataSet("MyExistingDatasetName");
    Writing Data:
        Full Overwrite: If you write data to the entire dataset (using H5S_ALL for both memory and file dataspace selections or by simply calling dataset.write(data_buffer, ...) without explicit selections), it will overwrite the existing data within that dataset. The size and shape of the dataset do not change, but its contents are replaced.
 */

// Assume dataset already exists in the file with dimensions (10, 5)
std::vector<int> newData(50, 99); // Fill with new data
dataset.write(newData.data(), H5::PredType::NATIVE_INT); // Overwrites all 50 elements


/**
 * Partial Overwrite (Hyperslab Selection): This is how you "append" or "update" portions of an existing dataset.
 * You define a "hyperslab" (a sub-region) in both the memory buffer and the file dataset, 
 * and then write. This will overwrite only the specified region of the dataset, * leaving other parts untouched. 
 * This is effectively an "in-place update" or "partial append."
 */
// Assume dataset already exists with dimensions (100)
H5::DataSpace file_dataspace = dataset.getSpace();
hsize_t offset[1] = {50}; // Start writing from index 50
hsize_t count[1] = {10};  // Write 10 elements
file_dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);

H5::DataSpace mem_dataspace(1, count); // Memory dataspace for 10 elements

std::vector<int> partialData(10, 77);
dataset.write(partialData.data(), H5::PredType::NATIVE_INT, mem_dataspace, file_dataspace); // Overwrites indices 50-59


/**
 * Appending by Resizing (Extensible Datasets): If your dataset was created with H5S_UNLIMITED 
 * (or a larger maxdims) for one or more dimensions,
 * you can extend the dataset's size using dataset.extend() 
 * and then write to the newly allocated space using a hyperslab selection.
 * This is the true "append" functionality for growing datasets.
 */

// Assume dataset created with maxdims {H5S_UNLIMITED} and current dims {100}
hsize_t new_dims[1] = {120}; // Extend to 120 elements
dataset.extend(new_dims);

H5::DataSpace file_dataspace = dataset.getSpace();
hsize_t offset[1] = {100}; // Start writing at the end of original data
hsize_t count[1] = {20};   // Write 20 new elements
file_dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);

H5::DataSpace mem_dataspace(1, count);
std::vector<int> appendedData(20, 55);
dataset.write(appendedData.data(), H5::PredType::NATIVE_INT, mem_dataspace, file_dataspace); // Appends 20 new elements


/**
 * Conclusion:
    Creating a dataset with an existing name will always fail (throw an exception) 
    unless the file was initially truncated.
    Writing data to an existing dataset (using DataSet::write()) will overwrite the data in the target region. 
    Whether it's the whole dataset or a partial selection depends on your dataspace selections.
    True "appending" (growing the dataset) requires the dataset to be created with extensible dimensions (H5S_UNLIMITED)
     and then using DataSet::extend() followed by a hyperslab write.
 */
