#include <STEPCAFControl_Reader.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Tool.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Type.hxx>

#include <iostream>
#include <string>

// Utility to print color
std::string ColorToString(const Quantity_Color& color) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "RGB(%.2f, %.2f, %.2f)", color.Red(), color.Green(), color.Blue());
    return std::string(buffer);
}

// Recursively traverse the shape tree and print attributes
void PrintShapeAttributes(const TDF_Label& label,
                          const Handle(XCAFDoc_ColorTool)& colorTool,
                          int indent = 0) {
    // Indentation for visual hierarchy
    std::string pad(indent, ' ');

    // Print name
    Handle(TDataStd_Name) nameAttr;
    if (label.FindAttribute(TDataStd_Name::GetID(), nameAttr)) {
        TCollection_ExtendedString extStr = nameAttr->Get();
        std::wcout << std::wstring(pad.begin(), pad.end()) << L"Name: " << extStr.ToWideString() << std::endl;
    }

    // Get shape from label
    TopoDS_Shape shape;
    if (XCAFDoc_DocumentTool::ShapeTool(label)->GetShape(label, shape)) {
        if (!shape.IsNull()) {
            // Compute area if it is a face or shell
            GProp_GProps props;
            BRepGProp::SurfaceProperties(shape, props);
            double area = props.Mass();
            if (area > 1e-6) {
                std::cout << pad << "Area: " << area << std::endl;
            }

            // Try to get color
            Quantity_Color color;
            if (colorTool->GetColor(shape, XCAFDoc_ColorGen, color)) {
                std::cout << pad << "Color: " << ColorToString(color) << std::endl;
            }
        }
    }

    // Recurse into children
    TDF_LabelSequence children;
    label.Children(children);
    for (Standard_Integer i = 1; i <= children.Length(); ++i) {
        PrintShapeAttributes(children.Value(i), colorTool, indent + 2);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: step_attr_reader file.step\n";
        return 1;
    }

    std::string stepFile = argv[1];

    // Init XDE document
    Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
    Handle(TDocStd_Document) doc;
    app->NewDocument("MDTV-XCAF", doc);

    // Read STEP file with attribute support
    STEPCAFControl_Reader reader;
    reader.SetColorMode(true);
    reader.SetNameMode(true);
    reader.SetLayerMode(true);

    IFSelect_ReturnStatus status = reader.ReadFile(stepFile.c_str());
    if (status != IFSelect_RetDone) {
        std::cerr << "Error reading STEP file.\n";
        return 1;
    }

    if (!reader.Transfer(doc)) {
        std::cerr << "Error transferring STEP to document.\n";
        return 1;
    }

    // Get tools
    Handle(XCAFDoc_ColorTool) colorTool = XCAFDoc_DocumentTool::ColorTool(doc->Main());
    Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());

    // Get top-level shapes
    TDF_LabelSequence shapeLabels;
    shapeTool->GetFreeShapes(shapeLabels);

    std::cout << "Found " << shapeLabels.Length() << " top-level shapes.\n";

    for (Standard_Integer i = 1; i <= shapeLabels.Length(); ++i) {
        std::cout << "\nShape " << i << ":\n";
        PrintShapeAttributes(shapeLabels.Value(i), colorTool);
    }

    return 0;
}
