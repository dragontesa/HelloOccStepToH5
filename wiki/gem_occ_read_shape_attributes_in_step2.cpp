#include <iostream>
#include <string>
#include <vector>
#include <fstream> // For creating dummy STEP file
#include <iomanip> // For std::fixed and std::setprecision

// OpenCASCADE Headers for STEP reading and XCAF
#include <STEPCAFControl_Reader.hxx> // Use STEPCAFControl_Reader for ReadNames
#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx> // Needed for GetAssemblyRoot

// OpenCASCADE Headers for Geometry and Properties
#include <TopoDS_Shape.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopExp_Explorer.hxx> // For exploring subshapes

// Helper to convert TCollection_ExtendedString to std::string
std::string OCAFStringToString(const TCollection_ExtendedString& extendedString) {
    // Check if the extended string is empty or null to prevent issues
    if (extendedString.IsEmpty()) {
        return "(Empty Name String)";
    }
    char* cstr = new char[extendedString.LengthOfCString() + 1];
    extendedString.ToCString(cstr);
    std::string result(cstr);
    delete[] cstr;
    return result;
}

// Function to read a STEP file and populate an XCAF document
Handle(TDocStd_Document) ReadSTEPFile(const std::string& filepath) {
    Handle(TDocStd_Document) doc;
    // Use STEPCAFControl_Reader for more comprehensive XCAF data transfer, including names.
    STEPCAFControl_Reader reader;

    // Read the file
    IFSelect_ReturnStatus status = reader.ReadFile(filepath.c_str());

    if (status == IFSelect_RetDone) {
        std::cout << "STEP file '" << filepath << "' read successfully." << std::endl;

        // Get the XCAF application (create if it doesn't exist)
        Handle(XCAFApp_Application) app = XCAFApp_Application::Get();
        if (app.IsNull()) {
            std::cerr << "Error: XCAF application is null. Attempting to initialize for robustness." << std::endl;
            // Robust initialization for XCAF application
            app = new XCAFApp_Application();
            XCAFApp_Application::SetApplication(app); // Set it globally if it was null
        }

        // Create a new XCAF document
        app->NewDocument("XCAF", doc);

        // Transfer STEP data into the XCAF document
        // This transfers the geometric and topological data.
        reader.Transfer(doc);

        // Read names and other textual attributes into the XCAF document
        // This is the specific method you asked about.
        reader.ReadNames(doc);

        if (doc.IsNull()) {
            std::cerr << "Error: XCAF document is null after transfer." << std::endl;
        } else {
            std::cout << "STEP data transferred to XCAF document." << std::endl;
            std::cout << "Names and other textual attributes read into XCAF document." << std::endl;
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
    // Correct usage: GetLabelName returns TCollection_ExtendedString
    TCollection_ExtendedString name = shapeTool->GetLabelName(label);
    if (!name.IsEmpty()) { // Check if a name was actually retrieved
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
            // These calls might compute properties for the entire compound based on its constituents.
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
        dummyStepFile.close();
        std::cout << "Dummy STEP file '" << dummyStepFilename << "' created." << std::endl;
    } else {
        std::cerr << "Failed to create dummy STEP file." << std::endl;
        return 1;
    }

    // --- Initialize OpenCASCADE Application ---
    Handle(XCAFApp_Application) app = XCAFApp_Application::Get();
    if (app.IsNull()) {
        std::cerr << "Warning: XCAF application was null. Initializing." << std::endl;
        app = new XCAFApp_Application();
        XCAFApp_Application::SetApplication(app);
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
    // Also useful for general document queries, including product definitions
    Handle(XCAFDoc_DocumentTool) docTool = XCAFDoc_DocumentTool::Make(xcafDoc->Main());


    // --- Iterate through top-level Products (using GetProducts) ---
    TDF_LabelSequence productLabels;
    shapeTool->GetProducts(productLabels); // This correctly gets labels for PRODUCT entities

    if (productLabels.IsEmpty()) {
        std::cout << "No top-level Products/Assemblies found in the XCAF document." << std::endl;
    } else {
        std::cout << "\n--- Extracting Attributes for Products ---" << std::endl;
        for (int i = 1; i <= productLabels.Length(); ++i) {
            const TDF_Label& currentProductLabel = productLabels.Value(i);

            std::cout << "\nProcessing Product " << i << ": Label ID = " << currentProductLabel.Tag() << std::endl;

            // 1. Get Product Name
            TCollection_ExtendedString productName = shapeTool->GetLabelName(currentProductLabel);
            if (!productName.IsEmpty()) {
                std::cout << "  Product Name: " << OCAFStringToString(productName) << std::endl;
            } else {
                std::cout << "  Product Name: (Unnamed Product Label)" << std::endl;
            }

            // 2. Get Product Definition's ID (typically linked to the Product Label)
            // XCAFDoc_ShapeTool::GetProducts gives labels of PRODUCTS.
            // PRODUCT_DEFINITIONs are associated with PRODUCTS.
            // XCAFDoc_DocumentTool can help navigate from a PRODUCT to its PRODUCT_DEFINITION,
            // though the name itself is often directly attached to the PRODUCT label.
            // The 'ID' from PRODUCT_DEFINITION is often the key in this relationship.
            // You can iterate through attributes on the product label or its child labels
            // if you need to find specific TDataStd_Name attributes related to definitions.
            // For example, if you need the 'MainPart_ID' from PRODUCT_DEFINITION:
            // This kind of information is usually obtained by exploring attributes of the product label
            // or by navigating specific XCAF attributes if the STEP schema maps it directly.
            // For simple cases, the main name (like "MainPart") is usually sufficient.

            // Get the shape associated with this product (if any)
            TopoDS_Shape productShape;
            if (shapeTool->GetShape(currentProductLabel, productShape)) {
                std::cout << "  Associated Shape Found." << std::endl;
                GetAndPrintShapeAttributes(currentProductLabel, productShape, shapeTool, colorTool, 1);

                // Recursively explore components (children) of this shape/product
                TDF_LabelSequence components;
                shapeTool->GetComponents(currentProductLabel, components); // Use product label for components
                if (!components.IsEmpty()) {
                    std::cout << std::string(2, ' ') << "  Components for " << OCAFStringToString(shapeTool->GetLabelName(currentProductLabel)) << ":" << std::endl;
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
                std::cout << "  No geometric shape directly associated with this product label." << std::endl;
            }
        }
    }

    // Clean up dummy file
    std::remove(dummyStepFilename.c_str());
    std::cout << "\nCleaned up dummy STEP file." << std::endl;

    return 0;
}
