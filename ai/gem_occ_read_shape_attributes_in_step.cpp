#include <iostream>
#include <string>
#include <vector>
#include <fstream> // For creating dummy STEP file
#include <iomanip> // For std::fixed and std::setprecision

// OpenCASCADE Headers for STEP reading and XCAF
#include <STEPControl_Reader.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

// OpenCASCADE Headers for Geometry and Properties
#include <TopoDS_Shape.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopExp_Explorer.hxx> // For exploring subshapes

// Helper to convert TCollection_ExtendedString to std::string
std::string OCAFStringToString(const TCollection_ExtendedString& extendedString) {
    char* cstr = new char[extendedString.LengthOfCString() + 1];
    extendedString.ToCString(cstr);
    std::string result(cstr);
    delete[] cstr;
    return result;
}

// Function to read a STEP file and populate an XCAF document
Handle(TDocStd_Document) ReadSTEPFile(const std::string& filepath) {
    Handle(TDocStd_Document) doc;
    STEPControl_Reader reader;

    // Read the file
    IFSelect_ReturnStatus status = reader.ReadFile(filepath.c_str());

    if (status == IFSelect_RetDone) {
        std::cout << "STEP file '" << filepath << "' read successfully." << std::endl;

        // Get the XCAF application (create if it doesn't exist)
        Handle(XCAFApp_Application) app = XCAFApp_Application::Get();
        if (app.IsNull()) {
            std::cerr << "Error: XCAF application is null, attempting to initialize." << std::endl;
            // You might need to call XCAFApp_Application::Init() if Get() returns null initially
            // XCAFApp_Application::Init();
            // app = XCAFApp_Application::Get();
        }

        // Create a new XCAF document
        app->NewDocument("XCAF", doc);

        // Transfer STEP data into the XCAF document
        reader.Transfer(doc);

        if (doc.IsNull()) {
            std::cerr << "Error: XCAF document is null after transfer." << std::endl;
        } else {
            std::cout << "STEP data transferred to XCAF document." << std::endl;
        }
    } else {
        std::cerr << "Error reading STEP file '" << filepath << "'. Status: " << status << std::endl;
    }
    return doc;
}

// Function to extract and print attributes of a given shape
void GetAndPrintShapeAttributes(const TDF_Label& label, const TopoDS_Shape& shape,
                               Handle(XCAFDoc_ShapeTool) shapeTool,
                               Handle(XCAFDoc_ColorTool) colorTool,
                               int indent = 0) {
    // Indentation for hierarchical printing
    std::string indentStr(indent * 2, ' ');
    std::cout << std::fixed << std::setprecision(4); // For consistent float output

    std::cout << indentStr << "--- Shape Attributes ---" << std::endl;

    // 1. Get Shape Name
    TCollection_ExtendedString name;
    if (shapeTool->GetLabelName(label, name)) {
        std::cout << indentStr << "  Name: " << OCAFStringToString(name) << std::endl;
    } else {
        std::cout << indentStr << "  Name: (Unnamed or No Name Attached)" << std::endl;
    }

    // 2. Get Color
    Quantity_Color color;
    if (colorTool->GetColor(label, color)) {
        std::cout << indentStr << "  Color (RGB): R=" << color.Red()
                  << ", G=" << color.Green() << ", B=" << color.Blue() << std::endl;
    } else {
        std::cout << indentStr << "  Color: (Not Defined or No Color Attached)" << std::endl;
    }

    // 3. Get Area/Volume/Length (Geometric Properties)
    GProp_GProps props;
    double volume = 0.0, area = 0.0, length = 0.0;
    bool propertiesCalculated = false;

    // Determine shape type and calculate properties
    switch (shape.ShapeType()) {
        case TopAbs_SOLID:
        case TopAbs_COMPSOLID:
            BRepGProp::VolumeProperties(shape, props);
            volume = props.Volume();
            std::cout << indentStr << "  Volume: " << volume << std::endl;
            propertiesCalculated = true;
            break;
        case TopAbs_SHELL:
        case TopAbs_FACE:
            BRepGProp::SurfaceProperties(shape, props);
            area = props.Surface();
            std::cout << indentStr << "  Area: " << area << std::endl;
            propertiesCalculated = true;
            break;
        case TopAbs_WIRE:
        case TopAbs_EDGE:
            BRepGProp::LinearProperties(shape, props);
            length = props.Mass(); // For linear properties, Mass() returns the length
            std::cout << indentStr << "  Length: " << length << std::endl;
            propertiesCalculated = true;
            break;
        case TopAbs_COMPOUND:
            // For compounds, you might need to iterate through sub-shapes
            // and sum up their properties if composite property calc isn't sufficient.
            // BRepGProp can also work on compounds, but its meaning might be aggregate.
            BRepGProp::VolumeProperties(shape, props); // Try volume properties as a general fallback
            volume = props.Volume();
            BRepGProp::SurfaceProperties(shape, props); // Try surface properties
            area = props.Surface();
            std::cout << indentStr << "  Compound Volume (aggregate): " << volume << std::endl;
            std::cout << indentStr << "  Compound Area (aggregate): " << area << std::endl;
            propertiesCalculated = true;
            break;
        default:
            // For other types like TopAbs_VERTEX or empty shapes
            std::cout << indentStr << "  Geometric Properties: (Not applicable for this shape type: "
                      << shape.ShapeType() << ")" << std::endl;
            break;
    }

    if (propertiesCalculated) {
        // Centroid is also available if properties were calculated
        std::cout << indentStr << "  Centroid (CoM): X=" << props.CentreOfMass().X()
                  << ", Y=" << props.CentreOfMass().Y()
                  << ", Z=" << props.CentreOfMass().Z() << std::endl;
    }
    std::cout << std::endl;
}

// Main function to read STEP and process shapes
int main() {
    // --- Create a dummy STEP file for demonstration ---
    // This dummy file includes basic product structure.
    // For color and more complex attributes, real STEP files (e.g., AP214/AP242)
    // would define them explicitly. This dummy file will show "Not Defined" for color.
    const std::string dummyStepFilename = "sample_design.step";
    std::ofstream dummyStepFile(dummyStepFilename);
    if (dummyStepFile.is_open()) {
        dummyStepFile << "ISO-10303-21;\nHEADER;\nFILE_DESCRIPTION(('STEP AP203'),'2;1');\n";
        dummyStepFile << "FILE_NAME('sample_design.step','2023-01-01T10:00:00',('Author'),('Organization'),'PreProcessor','None','');\n";
        dummyStepFile << "FILE_SCHEMA(('CONFIG_CONTROL_DESIGN'));\nENDSEC;\nDATA;\n";
        dummyStepFile << "#10=PRODUCT_DEFINITION_CONTEXT('Part Definition','mechanical','design');\n";
        dummyStepFile << "#20=PRODUCT_DEFINITION('MainPart_ID','Main Part','',#10);\n";
        dummyStepFile << "#30=PRODUCT('MainPart','Main Part Long Name','Main Part Description','MainPart_ID');\n";
        dummyStepFile << "#40=MANIFOLD_SOLID_BREP('','');\n"; // Placeholder for geometry
        dummyStepFile << "#50=SHAPE_REPRESENTATION('MainPart_Shape','',(#40));\n"; // Shape for Main Part
        dummyStepFile << "#60=PRODUCT_DEFINITION_SHAPE('','',#30);\n"; // Link product to its shape
        dummyStepFile << "ENDSEC;\nEND-ISO-10303-21;\n";
        dummyStepFile.close();
        std::cout << "Dummy STEP file '" << dummyStepFilename << "' created." << std::endl;
    } else {
        std::cerr << "Failed to create dummy STEP file." << std::endl;
        return 1;
    }

    // --- Initialize OpenCASCADE Application ---
    // This is crucial for XCAF to work correctly
    Handle(XCAFApp_Application) app = XCAFApp_Application::Get();
    if (app.IsNull()) {
        // In some environments, XCAFApp_Application::Get() might return null initially.
        // You might need to call XCAFApp_Application::Init() if not already done.
        std::cerr << "Warning: XCAF application was null. This might indicate an initialization issue." << std::endl;
        // For robust initialization:
        // Handle(XCAFApp_Application) XCAFApp = XCAFApp_Application::Get();
        // if (XCAFApp.IsNull()) { XCAFApp = new XCAFApp_Application(); }
    }


    // --- Read the STEP file ---
    Handle(TDocStd_Document) xcafDoc = ReadSTEPFile(dummyStepFilename);

    if (xcafDoc.IsNull()) {
        std::cerr << "Failed to load STEP file into XCAF document. Exiting." << std::endl;
        std::remove(dummyStepFilename.c_str());
        return 1;
    }

    // --- Get XCAF Tools ---
    Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_ShapeTool::Make(xcafDoc->Main());
    Handle(XCAFDoc_ColorTool) colorTool = XCAFDoc_ColorTool::Make(xcafDoc->Main());

    // --- Iterate through top-level shapes (products/assemblies) ---
    TDF_LabelSequence topLevelLabels;
    shapeTool->GetProducts(topLevelLabels); // Gets labels of top-level products/assemblies

    if (topLevelLabels.IsEmpty()) {
        std::cout << "No top-level products/assemblies found in the XCAF document." << std::endl;
    } else {
        std::cout << "\n--- Extracting Attributes for Shapes ---" << std::endl;
        for (int i = 1; i <= topLevelLabels.Length(); ++i) {
            const TDF_Label& currentLabel = topLevelLabels.Value(i);
            TopoDS_Shape currentShape;

            // Get the actual shape geometry associated with this label
            if (shapeTool->GetShape(currentLabel, currentShape)) {
                std::cout << "\nProcessing Top-Level Product/Shape " << i << ": Label ID = " << currentLabel.Tag() << std::endl;
                GetAndPrintShapeAttributes(currentLabel, currentShape, shapeTool, colorTool, 1);

                // Recursively explore components (children) of this shape
                TDF_LabelSequence components;
                shapeTool->GetComponents(currentLabel, components);
                if (!components.IsEmpty()) {
                    std::cout << std::string(2, ' ') << "  Components for " << OCAFStringToString(shapeTool->GetLabelName(currentLabel)) << ":" << std::endl;
                    for (int j = 1; j <= components.Length(); ++j) {
                        const TDF_Label& componentLabel = components.Value(j);
                        TopoDS_Shape componentShape;
                        if (shapeTool->GetShape(componentLabel, componentShape)) {
                            std::cout << std::string(4, ' ') << "  Component " << j << ": Label ID = " << componentLabel.Tag() << std::endl;
                            GetAndPrintShapeAttributes(componentLabel, componentShape, shapeTool, colorTool, 3);
                        }
                    }
                }
            } else {
                std::cout << "Top-Level Label " << i << " has no associated shape geometry." << std::endl;
            }
        }
    }

    // Clean up dummy file
    std::remove(dummyStepFilename.c_str());
    std::cout << "\nCleaned up dummy STEP file." << std::endl;

    return 0;
}
