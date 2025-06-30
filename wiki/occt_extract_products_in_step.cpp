//=======================================================================
//function : ReadNames
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::ReadNames(const Handle(XSControl_WorkSession) &WS,
                                                  const Handle(TDocStd_Document)& Doc,
                                                  const STEPCAFControl_DataMapOfPDExternFile& PDFileMap) const
{
  // get starting data
  const Handle(Interface_InterfaceModel) &Model = WS->Model();
  const Handle(XSControl_TransferReader) &TR = WS->TransferReader();
  const Handle(Transfer_TransientProcess) &TP = TR->TransientProcess();
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  if (STool.IsNull()) return Standard_False;
  STEPConstruct_Tool Tool(WS);

  // iterate on model to find all SDRs and CDSRs
  Standard_Integer nb = Model->NbEntities();
  Handle(Standard_Type) tNAUO = STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence);
  Handle(Standard_Type) tPD = STANDARD_TYPE(StepBasic_ProductDefinition);
  Handle(Standard_Type) tPDWAD = STANDARD_TYPE(StepBasic_ProductDefinitionWithAssociatedDocuments);
  Handle(TCollection_HAsciiString) name;
  TDF_Label L;
  for (Standard_Integer i = 1; i <= nb; i++) {
    Handle(Standard_Transient) enti = Model->Value(i);

    // get description of NAUO
    if (enti->DynamicType() == tNAUO) {
      L.Nullify();
      Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO =
        Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(enti);
      if (NAUO.IsNull()) continue;
      Interface_EntityIterator subs = WS->Graph().Sharings(NAUO);
      for (subs.Start(); subs.More(); subs.Next()) {
        Handle(StepRepr_ProductDefinitionShape) PDS =
          Handle(StepRepr_ProductDefinitionShape)::DownCast(subs.Value());
        if (PDS.IsNull()) continue;
        Handle(StepBasic_ProductDefinitionRelationship) PDR = PDS->Definition().ProductDefinitionRelationship();
        if (PDR.IsNull()) continue;
        if (PDR->HasDescription() &&
          PDR->Description()->UsefullLength() > 0) name = PDR->Description();
        else if (!PDR->Name().IsNull() && PDR->Name()->UsefullLength() > 0) name = PDR->Name();
        else if (!PDR->Id().IsNull()) name = PDR->Id();
        else name = new TCollection_HAsciiString;
      }
      // find proper label
      L = FindInstance(NAUO, STool, Tool, myMap);
      if (L.IsNull()) continue;

      TCollection_ExtendedString str = convertName (name->String());
      TDataStd_Name::Set(L, str);
    }

    // for PD get name of associated product
    if (enti->DynamicType() == tPD || enti->DynamicType() == tPDWAD) {
      L.Nullify();
      Handle(StepBasic_ProductDefinition) PD =
        Handle(StepBasic_ProductDefinition)::DownCast(enti);
      if (PD.IsNull()) continue;
      Handle(StepBasic_Product) Prod = (!PD->Formation().IsNull() ? PD->Formation()->OfProduct() : NULL);
      if (Prod.IsNull())
        name = new TCollection_HAsciiString;
      else if (!Prod->Name().IsNull() && Prod->Name()->UsefullLength() > 0)
        name = Prod->Name();
      else if (!Prod->Id().IsNull())
        name = Prod->Id();
      else
        name = new TCollection_HAsciiString;
      L = GetLabelFromPD(PD, STool, TP, PDFileMap, myMap);
      if (L.IsNull()) continue;
      TCollection_ExtendedString str = convertName (name->String());
      TDataStd_Name::Set(L, str);
    }
    // set a name to the document
    //TCollection_ExtendedString str = convertName (name->String());
    //TDataStd_Name::Set ( L, str );
  }

  return Standard_True;
}