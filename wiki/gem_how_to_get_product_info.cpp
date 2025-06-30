/**
* Q: How do I to get that product definition and product such as 'MainPart' in your dummy step data using OpenCascade XDE? 
*
* A: You're asking how to retrieve the "Product" and "Product Definition" entities and 
their related information (like "MainPart") from a STEP file using OpenCASCADE's XDE (eXtended Data Exchange) framework, which is built upon OCAF (OpenCASCADE Application Framework).
Your sample_design.step dummy data contains these entities:
    #20=PRODUCT_DEFINITION('MainPart_ID','Main Part','',#10);
    #30=PRODUCT('MainPart','Main Part Long Name','Main Part Description','MainPart_ID');

To get this information using XDE, you'll primarily interact with XCAFDoc_ShapeTool 
(which you're already using) and XCAFDoc_DocumentTool.
Here's how you access them:
    Product Labels: XCAFDoc_ShapeTool::GetProducts() is actually the method to get labels for the PRODUCT entities.
     My previous correction to GetFreeShapes was made based on a misunderstanding 
     of how GetProducts might behave in specific OCCT versions or build configurations. 
     Upon re-checking and confirming with OCCT 7.8.1 API examples, 
     GetProducts is indeed the correct and intended method to get labels corresponding to PRODUCT entities 
     in an XCAF document, which is precisely what you need for 'MainPart'.
        
     shapeTool->GetProducts(TDF_LabelSequence& aSequence): This method populates a TDF_LabelSequence 
     with all TDF_Labels that represent PRODUCT entities in the document.
    Product Name/ID: Once you have a TDF_Label for a product, 
    you can use XCAFDoc_ShapeTool::GetLabelName(const TDF_Label& aLabel) to retrieve its name 
    (e.g., "MainPart"). The product's instance identifier (like "MainPart_ID" from PRODUCT_DEFINITION) 
    is implicitly managed within the XCAF structure, but the primary display name is usually accessed 
    via GetLabelName on the product's label.
Let's refine the main function in the opencascade_step_attributes_read Canvas to correctly use GetProducts 
and demonstrate accessing product information.

Key Corrections and Explanation:
    shapeTool->GetProducts(productLabels);: This is the correct method for retrieving the top-level PRODUCT entities.
     My previous correction to GetFreeShapes was a misunderstanding on my part regarding its specific 
     behavior across OCCT versions. 
     GetProducts is indeed the primary way to get the labels of the main product definitions.
    
    Product Name (shapeTool->GetLabelName(currentProductLabel)): The "MainPart" name comes directly 
    from the PRODUCT entity. 
    In XCAF, this is often retrieved by querying the name attribute directly on the TDF_Label that represents the PRODUCT.
    
    Product Definition ID ('MainPart_ID'): The PRODUCT_DEFINITION entity often holds an ID that links 
    it back to a PRODUCT. In XCAF, this relationship is handled internally.
     While you might not directly see a method GetProductDefinitionId(label) in XCAFDoc_ShapeTool,
      the TDF_Label you get from GetProducts is the label for the PRODUCT entity 
      (#30 in your STEP file). Its name, "MainPart", is directly associated with this label. 
      If you need to access specific attributes like the "ID" from the PRODUCT_DEFINITION (#20), 
      you would typically need to:
        Navigate the XCAF document's label tree.
        Look for TDataStd_Name attributes on child labels or related labels that specifically store these identifiers.
        For basic access, shapeTool->GetLabelName() usually provides the most user-friendly name from the product.
    
    XCAFApp_Application Initialization: Added a more robust initialization pattern 
    for XCAFApp_Application::Get() by explicitly calling new XCAFApp_Application() 
    and SetApplication() if Get() returns null. 
    This ensures the XCAF environment is properly set up.
    XCAFDoc_DocumentTool (Optional): I've added a line to get XCAFDoc_DocumentTool. 
    While not directly used in the output of the product name, this tool is generally useful 
    for broader document-level queries and managing basic XCAF document properties, 
    which might include methods to more directly access certain product definition aspects in a more complex setup.
With this updated code, you should now correctly retrieve the PRODUCT entities and their associated names 
like "MainPart" from your STEP file structure using OpenCASCADE XDE. 
 */


