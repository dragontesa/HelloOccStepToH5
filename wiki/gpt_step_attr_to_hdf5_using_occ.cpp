#include <STEPCAFControl_Reader.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Tool.hxx>
#include <TDF_ChildIterator.hxx>

#include <H5Cpp.h>

#include <iostream>
#include <string>

void WriteLabelToHDF5(const TDF_Label& label, H5::Group& group) {
    if (label.IsNull()) return;

    // Try to get the name attribute
    Handle(TDataStd_Name) nameAttr;
    if (label.FindAttribute(TDataStd_Name::GetID(), nameAttr)) {
        TCollection_ExtendedString extStr = nameAttr->Get();
        std::string name = extStr.ToExtString();
        group.createAttribute("name", H5::StrType(H5::PredType::C_S1, H5T_VARIABLE), H5::DataSpace())
             .write(H5::StrType(H5::PredType::C_S1, H5T_VARIABLE), name);
    }

    // Recursively process child labels
    for (TDF_ChildIterator it(label, Standard_True); it.More(); it.Next()) {
        const TDF_Label& child = it.Value();
        std::string childName = "label_" + std::to_string(child.Tag());
        H5::Group childGroup = group.createGroup(childName);
        WriteLabelToHDF5(child, childGroup);
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: step2hdf5 input.step output.h5\n";
        return 1;
    }

    std::string stepFile = argv[1];
    std::string hdf5File = argv[2];

    // Initialize the OCCT XDE application
    Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
    Handle(TDocStd_Document) doc;
    app->NewDocument("MDTV-XCAF", doc);

    // Read STEP file
    STEPCAFControl_Reader reader;
    reader.SetColorMode(true);
    reader.SetNameMode(true);
    reader.SetLayerMode(true);

    IFSelect_ReturnStatus status = reader.ReadFile(stepFile.c_str());
    if (status != IFSelect_RetDone) {
        std::cerr << "Failed to read STEP file.\n";
        return 1;
    }

    if (!reader.Transfer(doc)) {
        std::cerr << "Failed to transfer STEP to XDE document.\n";
        return 1;
    }

    // Get the root label
    TDF_Label shapeLabel = XCAFDoc_DocumentTool::ShapeLabel(doc->Main());

    try {
        H5::H5File file(hdf5File, H5F_ACC_TRUNC);
        H5::Group rootGroup = file.createGroup("/properties");

        // Start recursive export
        WriteLabelToHDF5(shapeLabel, rootGroup);

        std::cout << "STEP attributes written to: " << hdf5File << "\n";
    } catch (H5::FileIException& e) {
        std::cerr << "HDF5 File Error: " << e.getCDetailMsg() << "\n";
        return 1;
    } catch (H5::Exception& e) {
        std::cerr << "HDF5 Error: " << e.getCDetailMsg() << "\n";
        return 1;
    }

    return 0;
}
