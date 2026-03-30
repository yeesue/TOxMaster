// NOTE: The following line conatains a tag indicating a bugfix. Do not remove it!
// BugFix {C4246A91-D7E5-11d3-8355-00105ABE320C}
// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// AutosarFolder_Dispatch wrapper class

class AutosarFolder_Dispatch : public COleDispatchDriver
{
public:
	AutosarFolder_Dispatch() {}		// Calls COleDispatchDriver default constructor
	AutosarFolder_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	AutosarFolder_Dispatch(const AutosarFolder_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsAutosarFolder();
	LPDISPATCH GetAsap2Project(LPCTSTR anItemName);
	VARIANT GetAsap2Projects();
	BOOL AddTextSegment(LPCTSTR textSegment, const VARIANT& views);
	BOOL AssignViewToTextSegment(LPCTSTR View, LPCTSTR textSegment);
	BOOL DeAssignViewFromTextSegment(LPCTSTR View, LPCTSTR textSegment);
	LPDISPATCH Edit();
	LPDISPATCH EditInFront();
	CString GetConfiguration();
	CString GetDate();
	CString GetNoteForTextSegment(LPCTSTR textSegment);
	VARIANT GetTextSegments();
	CString GetVersion();
	VARIANT GetViewsForTextSegment(LPCTSTR textSegment);
	BOOL IsComponent();
	BOOL IsEdition();
	BOOL IsVersion();
	BOOL RemoveTextSegment(LPCTSTR textSegment);
	BOOL ResetVersion();
	BOOL SetConfiguration(LPCTSTR configuration);
	BOOL SetEdition();
	BOOL SetNoteForTextSegment(LPCTSTR noteText, LPCTSTR textSegment);
	BOOL SetVersion(LPCTSTR version);
	LPDISPATCH Copy(LPCTSTR name);
	LPDISPATCH CopyToFolder(LPCTSTR name, LPDISPATCH dispatch_folder);
	BOOL ExportToFile(LPCTSTR fileName, BOOL multipleFiles, BOOL reference);
	BOOL GetAccessRightApplication();
	BOOL GetAccessRightCodeGeneration();
	BOOL GetAccessRightExecute();
	BOOL GetAccessRightRead();
	BOOL GetAccessRightWrite();
	VARIANT GetAllReferecedDataBaseItems();
	CString GetComment();
	BOOL GetDisallowImport();
	CString GetName();
	CString GetNameWithPath();
	CString GetOID();
	LPDISPATCH GetParentFolder();
	CString GetType();
	BOOL IsAsap2CanDB();
	BOOL IsAsap2Catalog();
	BOOL IsAsap2Item();
	BOOL IsAsap2Project();
	BOOL IsAsap2ProjectFolder();
	BOOL IsAscetContainer();
	BOOL IsAscetFolder();
	BOOL IsAscetNetwork();
	BOOL IsAscetProject();
	BOOL IsAscetProtocol();
	BOOL IsClass();
	BOOL IsCodeComponent();
	BOOL IsConditionalTable();
	BOOL IsCTBlock();
	BOOL IsCTComponent();
	BOOL IsDataBaseItem();
	BOOL IsDataSet();
	BOOL IsDiscreteComponent();
	BOOL IsEnumeration();
	BOOL IsExperimentEnvironment();
	BOOL IsODXProject();
	BOOL IsFolder();
	BOOL IsFunctionalComponent();
	BOOL IsHardwareConfiguration();
	BOOL IsIcon();
	BOOL IsIncaFolder();
	BOOL IsModule();
	BOOL IsProject();
	BOOL IsSignal();
	BOOL IsStateMachine();
	BOOL Move(LPDISPATCH dispatch_folder);
	BOOL SetAccessRightApplication(BOOL aBool);
	BOOL SetAccessRightCodeGeneration(BOOL aBool);
	BOOL SetAccessRightExecute(BOOL aBool);
	BOOL SetAccessRightRead(BOOL aBool);
	BOOL SetAccessRightWrite(BOOL aBool);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetDisallowImport(BOOL aBool);
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// DriveRecorderAction_Dispatch wrapper class

class DriveRecorderAction_Dispatch : public COleDispatchDriver
{
public:
	DriveRecorderAction_Dispatch() {}		// Calls COleDispatchDriver default constructor
	DriveRecorderAction_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	DriveRecorderAction_Dispatch(const DriveRecorderAction_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetLabel();
	CString GetDescriptionText();
	CString GetCategoryLabel();
	BOOL GetDefaultValueIsResultLoggingEnabled();
	void BeginFireNonBlockingAction(BOOL IsResultLoggingEnabled, LPCTSTR AbsoluteLogFilePathFileNamePrefix);
};
/////////////////////////////////////////////////////////////////////////////
// DServerDiagnosticExperimentAccess_Dispatch wrapper class

class DServerDiagnosticExperimentAccess_Dispatch : public COleDispatchDriver
{
public:
	DServerDiagnosticExperimentAccess_Dispatch() {}		// Calls COleDispatchDriver default constructor
	DServerDiagnosticExperimentAccess_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	DServerDiagnosticExperimentAccess_Dispatch(const DServerDiagnosticExperimentAccess_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetAvailableDiagnosticActions();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2AxisDescription_Dispatch wrapper class

class Asap2AxisDescription_Dispatch : public COleDispatchDriver
{
public:
	Asap2AxisDescription_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2AxisDescription_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2AxisDescription_Dispatch(const Asap2AxisDescription_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long MaxAxisPoints();
	long GetAddress();
	CString GetAddressAsHexString();
	LPDISPATCH GetCompuMethod();
	LPDISPATCH GetDataItem();
	CString GetDataType();
	double GetLowerBound();
	double GetUpperBound();
	BOOL IsArray();
	BOOL IsAxisPoints();
	BOOL IsCharacteristic();
	BOOL IsMatrix();
	BOOL IsMeasurement();
	BOOL IsScalar();
	BOOL SetCompuMethod(LPDISPATCH dispatch_compuMethod);
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2Characteristic_Dispatch wrapper class

class Asap2Characteristic_Dispatch : public COleDispatchDriver
{
public:
	Asap2Characteristic_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2Characteristic_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2Characteristic_Dispatch(const Asap2Characteristic_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetAxisDescriptions();
	BOOL IsCharacteristic();
	BOOL IsCurve();
	BOOL IsMap();
	long GetXSize();
	long GetYSize();
	long GetZSize();
	long GetAddress();
	CString GetAddressAsHexString();
	LPDISPATCH GetCompuMethod();
	LPDISPATCH GetDataItem();
	CString GetDataType();
	double GetLowerBound();
	double GetUpperBound();
	BOOL IsArray();
	BOOL IsAxisPoints();
	BOOL IsMatrix();
	BOOL IsMeasurement();
	BOOL IsScalar();
	BOOL SetCompuMethod(LPDISPATCH dispatch_compuMethod);
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2CompuMethod_Dispatch wrapper class

class Asap2CompuMethod_Dispatch : public COleDispatchDriver
{
public:
	Asap2CompuMethod_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2CompuMethod_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2CompuMethod_Dispatch(const Asap2CompuMethod_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetConversionType();
	CString GetType();
	CString GetUnit();
	BOOL SetUnit(LPCTSTR unit);
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2Element_Dispatch wrapper class

class Asap2Element_Dispatch : public COleDispatchDriver
{
public:
	Asap2Element_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2Element_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2Element_Dispatch(const Asap2Element_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2Function_Dispatch wrapper class

class Asap2Function_Dispatch : public COleDispatchDriver
{
public:
	Asap2Function_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2Function_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2Function_Dispatch(const Asap2Function_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetAllInputMeasurements();
	VARIANT GetAllLocalMeasurements();
	VARIANT GetAllOutputMeasurements();
	VARIANT GetAllSubFunctions();
	CString GetFunctionVersion();
	VARIANT GetAllCharacteristics();
	VARIANT GetAllDefinedCharacteristics();
	VARIANT GetAllLabels();
	VARIANT GetAllMeasurements();
	VARIANT GetAllReferencedCharacteristics();
	BOOL IsRoot();
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2Group_Dispatch wrapper class

class Asap2Group_Dispatch : public COleDispatchDriver
{
public:
	Asap2Group_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2Group_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2Group_Dispatch(const Asap2Group_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetAllReferencedMeasurements();
	VARIANT GetAllSubFunctions();
	VARIANT GetAllSubGroups();
	VARIANT GetAllCharacteristics();
	VARIANT GetAllDefinedCharacteristics();
	VARIANT GetAllLabels();
	VARIANT GetAllMeasurements();
	VARIANT GetAllReferencedCharacteristics();
	BOOL IsRoot();
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2Grouping_Dispatch wrapper class

class Asap2Grouping_Dispatch : public COleDispatchDriver
{
public:
	Asap2Grouping_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2Grouping_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2Grouping_Dispatch(const Asap2Grouping_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetAllCharacteristics();
	VARIANT GetAllDefinedCharacteristics();
	VARIANT GetAllLabels();
	VARIANT GetAllMeasurements();
	VARIANT GetAllReferencedCharacteristics();
	BOOL IsRoot();
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2Label_Dispatch wrapper class

class Asap2Label_Dispatch : public COleDispatchDriver
{
public:
	Asap2Label_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2Label_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2Label_Dispatch(const Asap2Label_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetAddress();
	CString GetAddressAsHexString();
	LPDISPATCH GetCompuMethod();
	LPDISPATCH GetDataItem();
	CString GetDataType();
	double GetLowerBound();
	double GetUpperBound();
	BOOL IsArray();
	BOOL IsAxisPoints();
	BOOL IsCharacteristic();
	BOOL IsMatrix();
	BOOL IsMeasurement();
	BOOL IsScalar();
	BOOL SetCompuMethod(LPDISPATCH dispatch_compuMethod);
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2Measurement_Dispatch wrapper class

class Asap2Measurement_Dispatch : public COleDispatchDriver
{
public:
	Asap2Measurement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2Measurement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2Measurement_Dispatch(const Asap2Measurement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	double GetAccuracy();
	long GetResolution();
	long GetXSize();
	long GetYSize();
	BOOL IsMeasurement();
	long GetAddress();
	CString GetAddressAsHexString();
	LPDISPATCH GetCompuMethod();
	LPDISPATCH GetDataItem();
	CString GetDataType();
	double GetLowerBound();
	double GetUpperBound();
	BOOL IsArray();
	BOOL IsAxisPoints();
	BOOL IsCharacteristic();
	BOOL IsMatrix();
	BOOL IsScalar();
	BOOL SetCompuMethod(LPDISPATCH dispatch_compuMethod);
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2Module_Dispatch wrapper class

class Asap2Module_Dispatch : public COleDispatchDriver
{
public:
	Asap2Module_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2Module_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2Module_Dispatch(const Asap2Module_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH CreateCompuMethod(LPCTSTR formulaType, LPCTSTR formulaName);
	VARIANT GetAllAvailableCompuMethodTypes();
	VARIANT GetAllCharacteristics();
	VARIANT GetAllCompuMethods();
	VARIANT GetAllElementNamesOfType(LPCTSTR aType);
	VARIANT GetAllFunctions();
	VARIANT GetAllGroups();
	VARIANT GetAllMeasurements();
	VARIANT GetAllRootFunctions();
	VARIANT GetAllRootGroups();
	VARIANT GetAllUnreferencedCharacteristics();
	LPDISPATCH GetAxisPointNamed(LPCTSTR aName);
	LPDISPATCH GetCharacteristicNamed(LPCTSTR aName);
	LPDISPATCH GetCompuMethod(LPCTSTR formulaName);
	CString GetDescriptionFileName();
	LPDISPATCH GetMeasurementNamed(LPCTSTR aName);
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2Project_Dispatch wrapper class

class Asap2Project_Dispatch : public COleDispatchDriver
{
public:
	Asap2Project_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2Project_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2Project_Dispatch(const Asap2Project_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH AddTopFolder(LPCTSTR name);
	VARIANT AllDataSets();
	LPDISPATCH Asap2Module();
	LPDISPATCH Asap2ModuleForDataSet(LPDISPATCH dispatch_dataSet);
	CString CopyDataSetNamed(LPCTSTR aDataSetName);
	LPDISPATCH DataSetForName(LPCTSTR folderPathName);
	BOOL DeleteDataSetNamed(LPCTSTR aDataSetName);
	VARIANT GetAllDataSets();
	VARIANT GetAllReadOnlyDataSets();
	VARIANT GetAllDataSetNames();
	VARIANT GetAllTopFolders();
	VARIANT GetAllWorkbases();
	VARIANT GetAvailableKeyVariablesListNames();
	CString GetStatusOfKeyVariablesListNamed(LPCTSTR aKeyVaiablesListName);
	BOOL SetStatusOfKeyVariablesListNamed(LPCTSTR aKeyVaiablesListName, LPCTSTR aPropertyId);
	BOOL CreateKeyVariablesList(LPCTSTR aKeyVariablesListName, LPCTSTR aLabFileName);
	BOOL ImportLabFileToKeyVariablesList(LPCTSTR aKeyVariablesListName, LPCTSTR aLabFileName);
	CString GetMasterDataSet();
	VARIANT GetSupportedIfdataList();
	LPDISPATCH GetTopFolderNamed(LPCTSTR name);
	BOOL IsAsap2Project();
	BOOL IsProject();
	LPDISPATCH MasterDataSet();
	LPDISPATCH ReadHexFile(LPCTSTR pathName, LPDISPATCH dispatch_parentFolder, LPCTSTR dataSetName);
	BOOL RemoveTopFolder(LPCTSTR name);
	BOOL WriteAsap2ToFile(LPCTSTR anA2lFileName);
	BOOL IsTypeEcuDescription();
	BOOL IsTypeCanMonitoring();
	BOOL IsTypeFlexRayMonitoring();
	BOOL IsTypeLinMonitoring();
	BOOL IsAsap2Item();
	BOOL AddTextSegment(LPCTSTR textSegment, const VARIANT& views);
	BOOL AssignViewToTextSegment(LPCTSTR View, LPCTSTR textSegment);
	BOOL DeAssignViewFromTextSegment(LPCTSTR View, LPCTSTR textSegment);
	LPDISPATCH Edit();
	LPDISPATCH EditInFront();
	CString GetConfiguration();
	CString GetDate();
	CString GetNoteForTextSegment(LPCTSTR textSegment);
	VARIANT GetTextSegments();
	CString GetVersion();
	VARIANT GetViewsForTextSegment(LPCTSTR textSegment);
	BOOL IsComponent();
	BOOL IsEdition();
	BOOL IsVersion();
	BOOL RemoveTextSegment(LPCTSTR textSegment);
	BOOL ResetVersion();
	BOOL SetConfiguration(LPCTSTR configuration);
	BOOL SetEdition();
	BOOL SetNoteForTextSegment(LPCTSTR noteText, LPCTSTR textSegment);
	BOOL SetVersion(LPCTSTR version);
	LPDISPATCH Copy(LPCTSTR name);
	LPDISPATCH CopyToFolder(LPCTSTR name, LPDISPATCH dispatch_folder);
	BOOL ExportToFile(LPCTSTR fileName, BOOL multipleFiles, BOOL reference);
	BOOL GetAccessRightApplication();
	BOOL GetAccessRightCodeGeneration();
	BOOL GetAccessRightExecute();
	BOOL GetAccessRightRead();
	BOOL GetAccessRightWrite();
	VARIANT GetAllReferecedDataBaseItems();
	CString GetComment();
	BOOL GetDisallowImport();
	CString GetName();
	CString GetNameWithPath();
	CString GetOID();
	LPDISPATCH GetParentFolder();
	CString GetType();
	BOOL IsAsap2CanDB();
	BOOL IsAsap2Catalog();
	BOOL IsAutosarFolder();
	BOOL IsAsap2ProjectFolder();
	BOOL IsAscetContainer();
	BOOL IsAscetFolder();
	BOOL IsAscetNetwork();
	BOOL IsAscetProject();
	BOOL IsAscetProtocol();
	BOOL IsClass();
	BOOL IsCodeComponent();
	BOOL IsConditionalTable();
	BOOL IsCTBlock();
	BOOL IsCTComponent();
	BOOL IsDataBaseItem();
	BOOL IsDataSet();
	BOOL IsDiscreteComponent();
	BOOL IsEnumeration();
	BOOL IsExperimentEnvironment();
	BOOL IsODXProject();
	BOOL IsFolder();
	BOOL IsFunctionalComponent();
	BOOL IsHardwareConfiguration();
	BOOL IsIcon();
	BOOL IsIncaFolder();
	BOOL IsModule();
	BOOL IsSignal();
	BOOL IsStateMachine();
	BOOL Move(LPDISPATCH dispatch_folder);
	BOOL SetAccessRightApplication(BOOL aBool);
	BOOL SetAccessRightCodeGeneration(BOOL aBool);
	BOOL SetAccessRightExecute(BOOL aBool);
	BOOL SetAccessRightRead(BOOL aBool);
	BOOL SetAccessRightWrite(BOOL aBool);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetDisallowImport(BOOL aBool);
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2ProjectFolder_Dispatch wrapper class

class Asap2ProjectFolder_Dispatch : public COleDispatchDriver
{
public:
	Asap2ProjectFolder_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2ProjectFolder_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2ProjectFolder_Dispatch(const Asap2ProjectFolder_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsAsap2ProjectFolder();
	LPDISPATCH AddSubFolder(LPCTSTR folderName);
	VARIANT BrowseComponent(LPCTSTR name);
	VARIANT BrowseDataBaseItem(LPCTSTR name);
	VARIANT BrowseSubFolder(LPCTSTR name);
	VARIANT GetAllComponents();
	VARIANT GetAllDataBaseItems();
	VARIANT GetAllSubFolders();
	LPDISPATCH GetComponent(LPCTSTR componentName);
	LPDISPATCH GetDataBaseItem(LPCTSTR itemName);
	LPDISPATCH GetSubFolder(LPCTSTR folderName);
	BOOL IsFolder();
	BOOL RemoveComponent(LPDISPATCH dispatch_comp);
	BOOL RemoveFolder(LPDISPATCH dispatch_folder, BOOL recursive);
	LPDISPATCH Copy(LPCTSTR name);
	LPDISPATCH CopyToFolder(LPCTSTR name, LPDISPATCH dispatch_folder);
	BOOL ExportToFile(LPCTSTR fileName, BOOL multipleFiles, BOOL reference);
	BOOL GetAccessRightApplication();
	BOOL GetAccessRightCodeGeneration();
	BOOL GetAccessRightExecute();
	BOOL GetAccessRightRead();
	BOOL GetAccessRightWrite();
	VARIANT GetAllReferecedDataBaseItems();
	CString GetComment();
	BOOL GetDisallowImport();
	CString GetName();
	CString GetNameWithPath();
	CString GetOID();
	LPDISPATCH GetParentFolder();
	CString GetType();
	BOOL IsAsap2CanDB();
	BOOL IsAsap2Catalog();
	BOOL IsAsap2Item();
	BOOL IsAsap2Project();
	BOOL IsAutosarFolder();
	BOOL IsAscetContainer();
	BOOL IsAscetFolder();
	BOOL IsAscetNetwork();
	BOOL IsAscetProject();
	BOOL IsAscetProtocol();
	BOOL IsClass();
	BOOL IsCodeComponent();
	BOOL IsComponent();
	BOOL IsConditionalTable();
	BOOL IsCTBlock();
	BOOL IsCTComponent();
	BOOL IsDataBaseItem();
	BOOL IsDataSet();
	BOOL IsDiscreteComponent();
	BOOL IsEnumeration();
	BOOL IsExperimentEnvironment();
	BOOL IsODXProject();
	BOOL IsFunctionalComponent();
	BOOL IsHardwareConfiguration();
	BOOL IsIcon();
	BOOL IsIncaFolder();
	BOOL IsModule();
	BOOL IsProject();
	BOOL IsSignal();
	BOOL IsStateMachine();
	BOOL Move(LPDISPATCH dispatch_folder);
	BOOL SetAccessRightApplication(BOOL aBool);
	BOOL SetAccessRightCodeGeneration(BOOL aBool);
	BOOL SetAccessRightExecute(BOOL aBool);
	BOOL SetAccessRightRead(BOOL aBool);
	BOOL SetAccessRightWrite(BOOL aBool);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetDisallowImport(BOOL aBool);
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2RationalCompuMethod_Dispatch wrapper class

class Asap2RationalCompuMethod_Dispatch : public COleDispatchDriver
{
public:
	Asap2RationalCompuMethod_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2RationalCompuMethod_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2RationalCompuMethod_Dispatch(const Asap2RationalCompuMethod_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT FormulaCoefficients();
	double GetC0();
	double GetC1();
	VARIANT GetFormulaCoefficients();
	VARIANT GetParameterValues();
	BOOL IsIdentity();
	BOOL IsLinear();
	BOOL SetC0(double c0);
	BOOL SetC1(double c1);
	BOOL SetFormulaCoefficients(const VARIANT& coeffs);
	CString GetConversionType();
	CString GetType();
	CString GetUnit();
	BOOL SetUnit(LPCTSTR unit);
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Asap2TableCompuMethod_Dispatch wrapper class

class Asap2TableCompuMethod_Dispatch : public COleDispatchDriver
{
public:
	Asap2TableCompuMethod_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Asap2TableCompuMethod_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Asap2TableCompuMethod_Dispatch(const Asap2TableCompuMethod_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetHexTab();
	VARIANT GetHexTabDouble();
	VARIANT GetPhysTab();
	VARIANT GetVerbalTab();
	BOOL IsInterpolatedTable();
	BOOL IsNotInterpolatedTable();
	BOOL IsVerbalTable();
	BOOL SetHexTabAndPhysTab(const VARIANT& hexTab, const VARIANT& physTab);
	CString GetConversionType();
	CString GetType();
	CString GetUnit();
	BOOL SetUnit(LPCTSTR unit);
	CString GetComment();
	CString GetName();
	BOOL Remove();
	BOOL Rename(LPCTSTR name);
	BOOL SetComment(LPCTSTR comment);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CarbDevice_Dispatch wrapper class

class CarbDevice_Dispatch : public COleDispatchDriver
{
public:
	CarbDevice_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CarbDevice_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CarbDevice_Dispatch(const CarbDevice_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsCarbDevice();
	LPDISPATCH SendCarbMessageByte(const VARIANT& data);
	LPDISPATCH SendCarbMessageShort(const VARIANT& data);
	LPDISPATCH SendCarbMessage(const VARIANT& data);
	LPDISPATCH SendMode1Service(long pid);
	LPDISPATCH SendMode2Service(long pid, long frameNumber);
	LPDISPATCH SendMode3Service();
	LPDISPATCH SendMode4Service();
	LPDISPATCH SendMode5Service(long testId, long oxygenSensor);
	LPDISPATCH SendMode6Service(long testId);
	LPDISPATCH SendMode7Service();
	LPDISPATCH SendMode8Service(long testId, const VARIANT& data);
	LPDISPATCH SendMode8Service_2(long testId, long dataA, long dataB, long dataC, long dataD, long dataE);
	LPDISPATCH SendMode9Service(long infoType);
	VARIANT ActivateRci2ForAcquisitionRate(LPCTSTR name);
	BOOL CanAssignProject();
	BOOL DeActivateRci2ForAcquisitionRate(LPCTSTR name);
	double GetA2lScalingFactorOfSignalGroup(LPCTSTR signalGroupName);
	long GetA2lScalingUnitOfSignalGroup(LPCTSTR signalGroupName);
	VARIANT GetAllAcquisitionRates();
	LPDISPATCH GetASAP2Module();
	CString GetDeviceType();
	CString GetName();
	CString GetProjectDataBasePath();
	double GetSampleRateFor(LPCTSTR aName);
	BOOL IsAcquisitionRateAvailable(LPCTSTR rate);
	BOOL SetAsyncBuffersToInvalid();
	BOOL IsActive();
	BOOL IsCANMonitoringDevice();
	BOOL IsExperimentDevice();
	BOOL IsKwp2000Device();
	BOOL IsOutputDevice();
	BOOL IsWorkbaseDevice();
	BOOL IsEtherCATSupported();
	BOOL IsFULISupported();
	BOOL ResetTarget();
	BOOL StartTargetOS();
	BOOL StopTargetOS();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CarbResponse_Dispatch wrapper class

class CarbResponse_Dispatch : public COleDispatchDriver
{
public:
	CarbResponse_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CarbResponse_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CarbResponse_Dispatch(const CarbResponse_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT EcuIds();
	LPDISPATCH GetResponseForEcuId(long ecuId);
	BOOL IsError();
	long NumberOfResponses();
};
/////////////////////////////////////////////////////////////////////////////
// IncaOnlineExperimentMcd3Ext_Dispatch wrapper class

class IncaOnlineExperimentMcd3Ext_Dispatch : public COleDispatchDriver
{
public:
	IncaOnlineExperimentMcd3Ext_Dispatch() {}		// Calls COleDispatchDriver default constructor
	IncaOnlineExperimentMcd3Ext_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IncaOnlineExperimentMcd3Ext_Dispatch(const IncaOnlineExperimentMcd3Ext_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetVariableInformation(LPCTSTR deviceName);
	CString GetVariableInformationTest(LPCTSTR deviceName, long count);
	LPUNKNOWN GetVariableInformationAsStream(LPCTSTR deviceName);
	LPUNKNOWN GetVariableInformationTestAsStream(LPCTSTR deviceName, long count);
};
/////////////////////////////////////////////////////////////////////////////
// ODXProject_Dispatch wrapper class

class ODXProject_Dispatch : public COleDispatchDriver
{
public:
	ODXProject_Dispatch() {}		// Calls COleDispatchDriver default constructor
	ODXProject_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ODXProject_Dispatch(const ODXProject_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsODXProject();
	BOOL AddTextSegment(LPCTSTR textSegment, const VARIANT& views);
	BOOL AssignViewToTextSegment(LPCTSTR View, LPCTSTR textSegment);
	BOOL DeAssignViewFromTextSegment(LPCTSTR View, LPCTSTR textSegment);
	LPDISPATCH Edit();
	LPDISPATCH EditInFront();
	CString GetConfiguration();
	CString GetDate();
	CString GetNoteForTextSegment(LPCTSTR textSegment);
	VARIANT GetTextSegments();
	CString GetVersion();
	VARIANT GetViewsForTextSegment(LPCTSTR textSegment);
	BOOL IsComponent();
	BOOL IsEdition();
	BOOL IsVersion();
	BOOL RemoveTextSegment(LPCTSTR textSegment);
	BOOL ResetVersion();
	BOOL SetConfiguration(LPCTSTR configuration);
	BOOL SetEdition();
	BOOL SetNoteForTextSegment(LPCTSTR noteText, LPCTSTR textSegment);
	BOOL SetVersion(LPCTSTR version);
	LPDISPATCH Copy(LPCTSTR name);
	LPDISPATCH CopyToFolder(LPCTSTR name, LPDISPATCH dispatch_folder);
	BOOL ExportToFile(LPCTSTR fileName, BOOL multipleFiles, BOOL reference);
	BOOL GetAccessRightApplication();
	BOOL GetAccessRightCodeGeneration();
	BOOL GetAccessRightExecute();
	BOOL GetAccessRightRead();
	BOOL GetAccessRightWrite();
	VARIANT GetAllReferecedDataBaseItems();
	CString GetComment();
	BOOL GetDisallowImport();
	CString GetName();
	CString GetNameWithPath();
	CString GetOID();
	LPDISPATCH GetParentFolder();
	CString GetType();
	BOOL IsAsap2CanDB();
	BOOL IsAsap2Catalog();
	BOOL IsAsap2Item();
	BOOL IsAsap2Project();
	BOOL IsAutosarFolder();
	BOOL IsAsap2ProjectFolder();
	BOOL IsAscetContainer();
	BOOL IsAscetFolder();
	BOOL IsAscetNetwork();
	BOOL IsAscetProject();
	BOOL IsAscetProtocol();
	BOOL IsClass();
	BOOL IsCodeComponent();
	BOOL IsConditionalTable();
	BOOL IsCTBlock();
	BOOL IsCTComponent();
	BOOL IsDataBaseItem();
	BOOL IsDataSet();
	BOOL IsDiscreteComponent();
	BOOL IsEnumeration();
	BOOL IsExperimentEnvironment();
	BOOL IsFolder();
	BOOL IsFunctionalComponent();
	BOOL IsHardwareConfiguration();
	BOOL IsIcon();
	BOOL IsIncaFolder();
	BOOL IsModule();
	BOOL IsProject();
	BOOL IsSignal();
	BOOL IsStateMachine();
	BOOL Move(LPDISPATCH dispatch_folder);
	BOOL SetAccessRightApplication(BOOL aBool);
	BOOL SetAccessRightCodeGeneration(BOOL aBool);
	BOOL SetAccessRightExecute(BOOL aBool);
	BOOL SetAccessRightRead(BOOL aBool);
	BOOL SetAccessRightWrite(BOOL aBool);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetDisallowImport(BOOL aBool);
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// DataSet_Dispatch wrapper class

class DataSet_Dispatch : public COleDispatchDriver
{
public:
	DataSet_Dispatch() {}		// Calls COleDispatchDriver default constructor
	DataSet_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	DataSet_Dispatch(const DataSet_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL ClearReadOnly();
	LPDISPATCH CopyToFolder(LPCTSTR name, LPDISPATCH dispatch_folder);
	BOOL CreateHexFile(LPCTSTR pathName);
	BOOL Delete();
	LPDISPATCH Duplicate();
	CString GetBinaryFileName();
	LPDISPATCH GetProject();
	BOOL IsDataSet();
	BOOL IsMasterData();
	BOOL IsReadOnly();
	BOOL Move(LPDISPATCH dispatch_folder);
	BOOL SetReadOnly();
	LPDISPATCH Copy(LPCTSTR name);
	BOOL ExportToFile(LPCTSTR fileName, BOOL multipleFiles, BOOL reference);
	BOOL GetAccessRightApplication();
	BOOL GetAccessRightCodeGeneration();
	BOOL GetAccessRightExecute();
	BOOL GetAccessRightRead();
	BOOL GetAccessRightWrite();
	VARIANT GetAllReferecedDataBaseItems();
	CString GetComment();
	BOOL GetDisallowImport();
	CString GetName();
	CString GetNameWithPath();
	CString GetOID();
	LPDISPATCH GetParentFolder();
	CString GetType();
	BOOL IsAsap2CanDB();
	BOOL IsAsap2Catalog();
	BOOL IsAsap2Item();
	BOOL IsAsap2Project();
	BOOL IsAutosarFolder();
	BOOL IsAsap2ProjectFolder();
	BOOL IsAscetContainer();
	BOOL IsAscetFolder();
	BOOL IsAscetNetwork();
	BOOL IsAscetProject();
	BOOL IsAscetProtocol();
	BOOL IsClass();
	BOOL IsCodeComponent();
	BOOL IsComponent();
	BOOL IsConditionalTable();
	BOOL IsCTBlock();
	BOOL IsCTComponent();
	BOOL IsDataBaseItem();
	BOOL IsDiscreteComponent();
	BOOL IsEnumeration();
	BOOL IsExperimentEnvironment();
	BOOL IsODXProject();
	BOOL IsFolder();
	BOOL IsFunctionalComponent();
	BOOL IsHardwareConfiguration();
	BOOL IsIcon();
	BOOL IsIncaFolder();
	BOOL IsModule();
	BOOL IsProject();
	BOOL IsSignal();
	BOOL IsStateMachine();
	BOOL SetAccessRightApplication(BOOL aBool);
	BOOL SetAccessRightCodeGeneration(BOOL aBool);
	BOOL SetAccessRightExecute(BOOL aBool);
	BOOL SetAccessRightRead(BOOL aBool);
	BOOL SetAccessRightWrite(BOOL aBool);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetDisallowImport(BOOL aBool);
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// EtasCDMToolbox_Dispatch wrapper class

class EtasCDMToolbox_Dispatch : public COleDispatchDriver
{
public:
	EtasCDMToolbox_Dispatch() {}		// Calls COleDispatchDriver default constructor
	EtasCDMToolbox_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	EtasCDMToolbox_Dispatch(const EtasCDMToolbox_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL ActivateDestination(LPCTSTR aCompleteDatasetPath, BOOL aFlag);
	BOOL AddAllLabelsToList();
	BOOL AddDestinationDataSet(LPCTSTR aCompleteDatasetPath);
	BOOL AddDestinationFile(LPCTSTR aFileName);
	BOOL AddLabelsFromFileToList(LPCTSTR aFileName);
	BOOL AddLabelsToList(const VARIANT& aLabelNameList);
	BOOL AddLabelsToListUsingMask(LPCTSTR aMask);
	BOOL AddFunctionsToListUsingMask(LPCTSTR aMask);
	BOOL Compare();
	BOOL Copy();
	BOOL ExchangeSourceWithDestination(LPCTSTR aCompleteDatasetPath);
	CString GetDifferenceFileName();
	CString GetErrorFileName();
	CString GetMissingLabelFileName();
	CString GetNewLabelFileName();
	long GetNumberOfDiffLabels();
	long GetNumberOfErrors();
	long GetNumberOfMissingLabels();
	long GetNumberOfNewLabels();
	long GetNumberOfWarnings();
	CString GetOutputFileName();
	CString GetWarningFileName();
	BOOL List();
	BOOL ListInDCMFormatToFile(LPCTSTR aFileName);
	BOOL LoadCDMConfigurationFromFile(LPCTSTR aFileName);
	BOOL LoadLabelListFromFile(LPCTSTR aFileName);
	BOOL ReadLabelListFromFile(LPCTSTR aFileName);
	BOOL RemoveAllDestinations();
	BOOL RemoveAllLabelsFromList();
	BOOL RemoveLabelsFromList(const VARIANT& aLabelNameList);
	BOOL RemoveLabelsFromListUsingMask(LPCTSTR aMask);
	BOOL RemoveSource();
	BOOL SaveCDMConfigurationToFile(LPCTSTR aFileName);
	BOOL SaveLabelListToFile(LPCTSTR aFileName);
	BOOL SetDestinationDataSet(LPCTSTR aCompleteDatasetPath);
	BOOL SetOutputFormat(LPCTSTR anOutputFormat);
	BOOL SetResultFilePath(LPCTSTR aPath);
	BOOL SetResultFilePrefix(LPCTSTR aPath);
	BOOL SetSourceDataSet(LPCTSTR aCompleteDatasetPath);
	BOOL SetSourceFile(LPCTSTR aFileName);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// ExperimentEnvironment_Dispatch wrapper class

class ExperimentEnvironment_Dispatch : public COleDispatchDriver
{
public:
	ExperimentEnvironment_Dispatch() {}		// Calls COleDispatchDriver default constructor
	ExperimentEnvironment_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ExperimentEnvironment_Dispatch(const ExperimentEnvironment_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetHardwareConfiguration();
	BOOL IsExperimentEnvironment();
	LPDISPATCH OpenExperiment();
	BOOL SetHardwareConfiguration(LPDISPATCH dispatch_hwk);
	BOOL AddTextSegment(LPCTSTR textSegment, const VARIANT& views);
	BOOL AssignViewToTextSegment(LPCTSTR View, LPCTSTR textSegment);
	BOOL DeAssignViewFromTextSegment(LPCTSTR View, LPCTSTR textSegment);
	LPDISPATCH Edit();
	LPDISPATCH EditInFront();
	CString GetConfiguration();
	CString GetDate();
	CString GetNoteForTextSegment(LPCTSTR textSegment);
	VARIANT GetTextSegments();
	CString GetVersion();
	VARIANT GetViewsForTextSegment(LPCTSTR textSegment);
	BOOL IsComponent();
	BOOL IsEdition();
	BOOL IsVersion();
	BOOL RemoveTextSegment(LPCTSTR textSegment);
	BOOL ResetVersion();
	BOOL SetConfiguration(LPCTSTR configuration);
	BOOL SetEdition();
	BOOL SetNoteForTextSegment(LPCTSTR noteText, LPCTSTR textSegment);
	BOOL SetVersion(LPCTSTR version);
	LPDISPATCH Copy(LPCTSTR name);
	LPDISPATCH CopyToFolder(LPCTSTR name, LPDISPATCH dispatch_folder);
	BOOL ExportToFile(LPCTSTR fileName, BOOL multipleFiles, BOOL reference);
	BOOL GetAccessRightApplication();
	BOOL GetAccessRightCodeGeneration();
	BOOL GetAccessRightExecute();
	BOOL GetAccessRightRead();
	BOOL GetAccessRightWrite();
	VARIANT GetAllReferecedDataBaseItems();
	CString GetComment();
	BOOL GetDisallowImport();
	CString GetName();
	CString GetNameWithPath();
	CString GetOID();
	LPDISPATCH GetParentFolder();
	CString GetType();
	BOOL IsAsap2CanDB();
	BOOL IsAsap2Catalog();
	BOOL IsAsap2Item();
	BOOL IsAsap2Project();
	BOOL IsAutosarFolder();
	BOOL IsAsap2ProjectFolder();
	BOOL IsAscetContainer();
	BOOL IsAscetFolder();
	BOOL IsAscetNetwork();
	BOOL IsAscetProject();
	BOOL IsAscetProtocol();
	BOOL IsClass();
	BOOL IsCodeComponent();
	BOOL IsConditionalTable();
	BOOL IsCTBlock();
	BOOL IsCTComponent();
	BOOL IsDataBaseItem();
	BOOL IsDataSet();
	BOOL IsDiscreteComponent();
	BOOL IsEnumeration();
	BOOL IsODXProject();
	BOOL IsFolder();
	BOOL IsFunctionalComponent();
	BOOL IsHardwareConfiguration();
	BOOL IsIcon();
	BOOL IsIncaFolder();
	BOOL IsModule();
	BOOL IsProject();
	BOOL IsSignal();
	BOOL IsStateMachine();
	BOOL Move(LPDISPATCH dispatch_folder);
	BOOL SetAccessRightApplication(BOOL aBool);
	BOOL SetAccessRightCodeGeneration(BOOL aBool);
	BOOL SetAccessRightExecute(BOOL aBool);
	BOOL SetAccessRightRead(BOOL aBool);
	BOOL SetAccessRightWrite(BOOL aBool);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetDisallowImport(BOOL aBool);
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// HardwareConfiguration_Dispatch wrapper class

class HardwareConfiguration_Dispatch : public COleDispatchDriver
{
public:
	HardwareConfiguration_Dispatch() {}		// Calls COleDispatchDriver default constructor
	HardwareConfiguration_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	HardwareConfiguration_Dispatch(const HardwareConfiguration_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH AddDevice(LPDISPATCH dispatch_dev);
	LPDISPATCH AddDeviceToSystem(LPDISPATCH dispatch_dev, LPDISPATCH dispatch_sys);
	LPDISPATCH AddSubDevice(LPDISPATCH dispatch_dev);
	LPDISPATCH AddSubDeviceToDevice(LPDISPATCH dispatch_devDes, LPDISPATCH dispatch_dev);
	LPDISPATCH AddSubDeviceToSystem(LPDISPATCH dispatch_dev, LPDISPATCH dispatch_sys);
	VARIANT GetAllSystems();
	LPDISPATCH GetAssignedExperimentEnvironment();
	LPDISPATCH GetSystem(LPCTSTR aName);
	BOOL HasAssignedExperimentEnvironment();
	BOOL InitializeHardware();
	BOOL IsHardwareConfiguration();
	BOOL RemoveDevice(LPDISPATCH dispatch_dev);
	BOOL RemoveSubDevice(LPDISPATCH dispatch_dev);
	BOOL RemoveSystem(LPDISPATCH dispatch_sys);
	BOOL SetExperimentEnvironment(LPDISPATCH dispatch_ee);
	BOOL AddTextSegment(LPCTSTR textSegment, const VARIANT& views);
	BOOL AssignViewToTextSegment(LPCTSTR View, LPCTSTR textSegment);
	BOOL DeAssignViewFromTextSegment(LPCTSTR View, LPCTSTR textSegment);
	LPDISPATCH Edit();
	LPDISPATCH EditInFront();
	CString GetConfiguration();
	CString GetDate();
	CString GetNoteForTextSegment(LPCTSTR textSegment);
	VARIANT GetTextSegments();
	CString GetVersion();
	VARIANT GetViewsForTextSegment(LPCTSTR textSegment);
	BOOL IsComponent();
	BOOL IsEdition();
	BOOL IsVersion();
	BOOL RemoveTextSegment(LPCTSTR textSegment);
	BOOL ResetVersion();
	BOOL SetConfiguration(LPCTSTR configuration);
	BOOL SetEdition();
	BOOL SetNoteForTextSegment(LPCTSTR noteText, LPCTSTR textSegment);
	BOOL SetVersion(LPCTSTR version);
	LPDISPATCH Copy(LPCTSTR name);
	LPDISPATCH CopyToFolder(LPCTSTR name, LPDISPATCH dispatch_folder);
	BOOL ExportToFile(LPCTSTR fileName, BOOL multipleFiles, BOOL reference);
	BOOL GetAccessRightApplication();
	BOOL GetAccessRightCodeGeneration();
	BOOL GetAccessRightExecute();
	BOOL GetAccessRightRead();
	BOOL GetAccessRightWrite();
	VARIANT GetAllReferecedDataBaseItems();
	CString GetComment();
	BOOL GetDisallowImport();
	CString GetName();
	CString GetNameWithPath();
	CString GetOID();
	LPDISPATCH GetParentFolder();
	CString GetType();
	BOOL IsAsap2CanDB();
	BOOL IsAsap2Catalog();
	BOOL IsAsap2Item();
	BOOL IsAsap2Project();
	BOOL IsAutosarFolder();
	BOOL IsAsap2ProjectFolder();
	BOOL IsAscetContainer();
	BOOL IsAscetFolder();
	BOOL IsAscetNetwork();
	BOOL IsAscetProject();
	BOOL IsAscetProtocol();
	BOOL IsClass();
	BOOL IsCodeComponent();
	BOOL IsConditionalTable();
	BOOL IsCTBlock();
	BOOL IsCTComponent();
	BOOL IsDataBaseItem();
	BOOL IsDataSet();
	BOOL IsDiscreteComponent();
	BOOL IsEnumeration();
	BOOL IsExperimentEnvironment();
	BOOL IsODXProject();
	BOOL IsFolder();
	BOOL IsFunctionalComponent();
	BOOL IsIcon();
	BOOL IsIncaFolder();
	BOOL IsModule();
	BOOL IsProject();
	BOOL IsSignal();
	BOOL IsStateMachine();
	BOOL Move(LPDISPATCH dispatch_folder);
	BOOL SetAccessRightApplication(BOOL aBool);
	BOOL SetAccessRightCodeGeneration(BOOL aBool);
	BOOL SetAccessRightExecute(BOOL aBool);
	BOOL SetAccessRightRead(BOOL aBool);
	BOOL SetAccessRightWrite(BOOL aBool);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetDisallowImport(BOOL aBool);
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// HWDevice_Dispatch wrapper class

class HWDevice_Dispatch : public COleDispatchDriver
{
public:
	HWDevice_Dispatch() {}		// Calls COleDispatchDriver default constructor
	HWDevice_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	HWDevice_Dispatch(const HWDevice_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetAllSubDevices();
	LPDISPATCH GetDeviceDescription();
	CString GetName();
	LPDISPATCH GetParentSystem();
	LPDISPATCH GetSubDevice(LPCTSTR aName);
	BOOL IsHWDevice();
	BOOL IsProjectDevice();
	BOOL IsWorkbaseDevice();
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDataBaseItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// HWDeviceDescription_Dispatch wrapper class

class HWDeviceDescription_Dispatch : public COleDispatchDriver
{
public:
	HWDeviceDescription_Dispatch() {}		// Calls COleDispatchDriver default constructor
	HWDeviceDescription_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	HWDeviceDescription_Dispatch(const HWDeviceDescription_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetAllSubDeviceDescriptions();
	CString GetName();
	LPDISPATCH GetParentSystemDescription();
	LPDISPATCH GetSubDeviceDescription(LPCTSTR aName);
	BOOL IsDeviceDescription();
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDataBaseItem();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// HWProjectDevice_Dispatch wrapper class

class HWProjectDevice_Dispatch : public COleDispatchDriver
{
public:
	HWProjectDevice_Dispatch() {}		// Calls COleDispatchDriver default constructor
	HWProjectDevice_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	HWProjectDevice_Dispatch(const HWProjectDevice_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsProjectDevice();
	BOOL SetProject(LPDISPATCH dispatch_prj);
	BOOL CanBeUsedWithProject(LPDISPATCH dispatch_prj);
	VARIANT GetAllSubDevices();
	LPDISPATCH GetDeviceDescription();
	CString GetName();
	LPDISPATCH GetParentSystem();
	LPDISPATCH GetSubDevice(LPCTSTR aName);
	BOOL IsHWDevice();
	BOOL IsWorkbaseDevice();
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDataBaseItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// HWSubDevice_Dispatch wrapper class

class HWSubDevice_Dispatch : public COleDispatchDriver
{
public:
	HWSubDevice_Dispatch() {}		// Calls COleDispatchDriver default constructor
	HWSubDevice_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	HWSubDevice_Dispatch(const HWSubDevice_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetName();
	LPDISPATCH GetParentDevice();
	LPDISPATCH GetSubDeviceDescription();
	BOOL IsHWSubDevice();
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDataBaseItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// HWSubDeviceDescription_Dispatch wrapper class

class HWSubDeviceDescription_Dispatch : public COleDispatchDriver
{
public:
	HWSubDeviceDescription_Dispatch() {}		// Calls COleDispatchDriver default constructor
	HWSubDeviceDescription_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	HWSubDeviceDescription_Dispatch(const HWSubDeviceDescription_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetName();
	LPDISPATCH GetParentDeviceDescription();
	BOOL IsSubDeviceDescription();
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDataBaseItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// HWSystem_Dispatch wrapper class

class HWSystem_Dispatch : public COleDispatchDriver
{
public:
	HWSystem_Dispatch() {}		// Calls COleDispatchDriver default constructor
	HWSystem_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	HWSystem_Dispatch(const HWSystem_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetAllDevices();
	VARIANT GetAllSystems();
	LPDISPATCH GetDevice(LPCTSTR aName);
	CString GetName();
	LPDISPATCH GetParentSystem();
	LPDISPATCH GetSystem(LPCTSTR aName);
	LPDISPATCH GetSystemDescription();
	BOOL IsHWSystem();
	BOOL SetName(LPCTSTR aName);
	BOOL SetProject(LPDISPATCH dispatch_prj);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDataBaseItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// HWSystemDescription_Dispatch wrapper class

class HWSystemDescription_Dispatch : public COleDispatchDriver
{
public:
	HWSystemDescription_Dispatch() {}		// Calls COleDispatchDriver default constructor
	HWSystemDescription_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	HWSystemDescription_Dispatch(const HWSystemDescription_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetAllDeviceDescriptions();
	VARIANT GetAllSystemDescriptions();
	LPDISPATCH GetDeviceDescription(LPCTSTR aName);
	CString GetName();
	LPDISPATCH GetParentSystemDescription();
	LPDISPATCH GetSystemDescription(LPCTSTR aName);
	BOOL IsSystemDescription();
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDataBaseItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// HWWorkbaseDevice_Dispatch wrapper class

class HWWorkbaseDevice_Dispatch : public COleDispatchDriver
{
public:
	HWWorkbaseDevice_Dispatch() {}		// Calls COleDispatchDriver default constructor
	HWWorkbaseDevice_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	HWWorkbaseDevice_Dispatch(const HWWorkbaseDevice_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsWorkbaseDevice();
	BOOL SetProjectAndDataSet(LPDISPATCH dispatch_prj, LPCTSTR aName);
	BOOL SetProjectAndWorkbase(LPDISPATCH dispatch_prj, LPCTSTR aName);
	BOOL SetWorkbase(LPCTSTR aName);
	BOOL SetODXProject(LPDISPATCH dispatch_prj);
	LPDISPATCH GetOdxFlashDevice();
	BOOL IsOdxSupport();
	BOOL IsConfiguredForOdxFlashing();
	BOOL QuickStart_Enable();
	BOOL QuickStart_Disable();
	BOOL IsProjectDevice();
	BOOL SetProject(LPDISPATCH dispatch_prj);
	BOOL CanBeUsedWithProject(LPDISPATCH dispatch_prj);
	VARIANT GetAllSubDevices();
	LPDISPATCH GetDeviceDescription();
	CString GetName();
	LPDISPATCH GetParentSystem();
	LPDISPATCH GetSubDevice(LPCTSTR aName);
	BOOL IsHWDevice();
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDataBaseItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Inca_Dispatch wrapper class

class Inca_Dispatch : public COleDispatchDriver
{
public:
	Inca_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Inca_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Inca_Dispatch(const Inca_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString APIVersion();
	VARIANT GetAllSystemDescriptions();
	long GetCurrentASAMMCD3Mode();
	LPDISPATCH GetCurrentDataBase();
	CString GetDataBasePath();
	double GetPollingCycleTimeTS();
	VARIANT GetRecordingFileFormats();
	VARIANT GetRecordingFileFormatsInternal();
	CString GetPrimaryRecordingFormatID();
	CString GetPrimaryRecordingFormatFileExtension();
	LPDISPATCH GetSystemDescription(LPCTSTR aName);
	BOOL IsWorkbaseActive();
	LPDISPATCH OpenDataBase(LPCTSTR DataBase);
	BOOL RegisterASAP3ProcId(long aProgId);
	BOOL SwitchToDataSet();
	BOOL SwitchToUserNamed(LPCTSTR userName);
	BOOL SwitchToWorkbase();
	CString ToolName();
	BOOL GetDriveRecorderMode();
	BOOL IsIndexAlignmentLeft();
	CString GetOption(LPCTSTR aParameterString);
	CString GetState(LPCTSTR aParameterString);
	BOOL SetOption(LPCTSTR aParameterString);
	CString GetOptionInformation(LPCTSTR aParameterString);
	CString GetStateInformation(LPCTSTR aParameterString);
	BOOL AddMenuButton(LPCTSTR aMenuButtonName, LPCTSTR aMenuName, LPCTSTR aViewName);
	BOOL ClearMonitor();
	BOOL CloseDatabase();
	BOOL CloseTool();
	long CreateSeed();
	BOOL DisconnectFromTool();
	void Exit(long exitNumber);
	VARIANT GetAllToolboxNames();
	CString GetApplicationDataPath();
	CString GetApplicationLanguage();
	CString GetApplicationProgramPath();
	CString GetApplicationSharedPath();
	LPDISPATCH GetCDMToolbox();
	CString GetCurrentVersion();
	CString GetETASLogFilePath();
	CString GetETASManualPath();
	CString GetETASTempPath();
	LPDISPATCH GetLicenseManager();
	LPDISPATCH GetOpenedExperiment();
	LPDISPATCH GetOpenedExperimentView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsDatabaseLocked();
	BOOL IsToolErrorAvailable();
	BOOL IsToolLocked();
	BOOL LockDatabase();
	BOOL LockTool();
	BOOL PermitMessageBoxes();
	CString ProgId();
	BOOL SuppressMessageBoxes();
	void SwitchCatchWalkbackOff();
	void SwitchCatchWalkbackOn();
	void SwitchLoggingOff();
	void SwitchLoggingOn();
	BOOL UnlockDatabase();
	BOOL UnlockTool();
	LPDISPATCH WaitMessage(LPCTSTR text);
	BOOL WriteMonitorLogFileToPath(LPCTSTR pathName);
	BOOL WriteMonitorLogFileToPathUsingFileName(LPCTSTR pathName, LPCTSTR fileName);
	BOOL WriteReadyToMonitor();
	BOOL WriteToMonitor(LPCTSTR text);
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// IncaDataBase_Dispatch wrapper class

class IncaDataBase_Dispatch : public COleDispatchDriver
{
public:
	IncaDataBase_Dispatch() {}		// Calls COleDispatchDriver default constructor
	IncaDataBase_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IncaDataBase_Dispatch(const IncaDataBase_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH AddIncaFolder(LPCTSTR folderName);
	LPDISPATCH GetActiveHardwareConfiguration();
	VARIANT GetAllIncaFolders();
	LPDISPATCH GetIncaFolder(LPCTSTR folderName);
	BOOL IsIncaDataBase();
	LPDISPATCH AddFolder(LPCTSTR folderName);
	VARIANT BrowseItem(LPCTSTR itemName);
	VARIANT BrowseItemInFolder(LPCTSTR itemName, LPCTSTR folderName);
	BOOL Close();
	void DisableAutoCommit();
	void EnableAutoCommit();
	BOOL ExportToFile(const VARIANT& items, LPCTSTR aFilePath, BOOL multipleFiles, BOOL withReferences);
	VARIANT GetAllFolders();
	LPDISPATCH GetFolder(LPCTSTR folderName);
	LPDISPATCH GetItem(LPCTSTR itemName);
	LPDISPATCH GetItemForOID(LPCTSTR oid);
	LPDISPATCH GetItemInFolder(LPCTSTR itemName, LPCTSTR folderName);
	CString GetName();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL ImportFromFile(LPCTSTR aFileName, BOOL overwriteFlag, BOOL discardImplFlag);
	VARIANT ImportItemsFromFile(LPCTSTR aFileName, BOOL overwriteFlag, BOOL discardImplFlag);
	BOOL IsAscetDataBase();
	BOOL IsDataBase();
	BOOL IsToolErrorAvailable();
	BOOL Remove(LPDISPATCH dispatch_aFolderItem, BOOL recursive);
	BOOL Save();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// IncaExperimentView_Dispatch wrapper class

class IncaExperimentView_Dispatch : public COleDispatchDriver
{
public:
	IncaExperimentView_Dispatch() {}		// Calls COleDispatchDriver default constructor
	IncaExperimentView_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IncaExperimentView_Dispatch(const IncaExperimentView_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetExperimentEnvironmentPathName();
	CString GetHardwareConfigurationPathName();
	BOOL IsIncaExperimentView();
	BOOL CloseAllAPICalibrationViews();
	BOOL CloseAllAPIMeasureViews();
	BOOL CloseAllViews();
	LPDISPATCH GetExperiment();
	BOOL IsExperimentChanged();
	BOOL IsExperimentView();
	BOOL IsGroupedDevices();
	BOOL OpenViewForExperimentDataItem(LPDISPATCH dispatch_edi);
	BOOL OpenViewForExperimentElement(LPDISPATCH dispatch_el);
	BOOL OpenViewForMeasureDataItemWithMaxLabels(LPDISPATCH dispatch_mdi, long amount);
	BOOL OpenViewForMeasureElementWithMaxLabels(LPDISPATCH dispatch_el, long amount);
	BOOL OpenViewsForExperimentDataItems(const VARIANT& edi);
	BOOL OpenViewsForExperimentElements(const VARIANT& el);
	BOOL OpenViewsForMeasureDataItemsWithMaxLabels(const VARIANT& mdi, long amount);
	BOOL OpenViewsForMeasureElementsWithMaxLabels(const VARIANT& el, long amount);
	BOOL Quit();
	BOOL SaveExperiment();
	BOOL SetGroupedDevices(long onOff);
	BOOL UpdateViewForExperimentDataItem(LPDISPATCH dispatch_edi);
	BOOL UpdateViewForExperimentElement(LPDISPATCH dispatch_el);
	BOOL UpdateViewsForExperimentDataItems(const VARIANT& edi);
	BOOL UpdateViewsForExperimentElements(const VARIANT& el);
	BOOL WriteCalibrationElementsToDCMFile(const VARIANT& edi, LPCTSTR fileName);
	BOOL BringToFront();
	BOOL Close();
	BOOL IsView();
	BOOL IsDialog();
	BOOL IsUserInterface();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// IncaFolder_Dispatch wrapper class

class IncaFolder_Dispatch : public COleDispatchDriver
{
public:
	IncaFolder_Dispatch() {}		// Calls COleDispatchDriver default constructor
	IncaFolder_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IncaFolder_Dispatch(const IncaFolder_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH AddExperimentEnvironment(LPCTSTR itemName);
	LPDISPATCH AddHardwareConfiguration(LPCTSTR itemName);
	LPDISPATCH AddIncaSubFolder(LPCTSTR folderName);
	VARIANT BrowseIncaSubFolder(LPCTSTR name);
	VARIANT GetAllIncaSubFolders();
	LPDISPATCH GetIncaSubFolder(LPCTSTR folderName);
	BOOL IsIncaFolder();
	LPDISPATCH ReadASAP2File(LPCTSTR fileName);
	LPDISPATCH ReadASAP2FileAndHexFile(LPCTSTR asap2FileName, LPCTSTR hexFileName);
	LPDISPATCH ReadCanDBFile(LPCTSTR fileName);
	LPDISPATCH ReadAutosarFile(LPCTSTR fileName);
	LPDISPATCH ReadFibexFile(LPCTSTR fileName);
	LPDISPATCH ReadLdfFile(LPCTSTR fileName);
	LPDISPATCH ReadODXProject(const VARIANT& fileNames);
	LPDISPATCH AddSubFolder(LPCTSTR folderName);
	VARIANT BrowseComponent(LPCTSTR name);
	VARIANT BrowseDataBaseItem(LPCTSTR name);
	VARIANT BrowseSubFolder(LPCTSTR name);
	VARIANT GetAllComponents();
	VARIANT GetAllDataBaseItems();
	VARIANT GetAllSubFolders();
	LPDISPATCH GetComponent(LPCTSTR componentName);
	LPDISPATCH GetDataBaseItem(LPCTSTR itemName);
	LPDISPATCH GetSubFolder(LPCTSTR folderName);
	BOOL IsFolder();
	BOOL RemoveComponent(LPDISPATCH dispatch_comp);
	BOOL RemoveFolder(LPDISPATCH dispatch_folder, BOOL recursive);
	LPDISPATCH Copy(LPCTSTR name);
	LPDISPATCH CopyToFolder(LPCTSTR name, LPDISPATCH dispatch_folder);
	BOOL ExportToFile(LPCTSTR fileName, BOOL multipleFiles, BOOL reference);
	BOOL GetAccessRightApplication();
	BOOL GetAccessRightCodeGeneration();
	BOOL GetAccessRightExecute();
	BOOL GetAccessRightRead();
	BOOL GetAccessRightWrite();
	VARIANT GetAllReferecedDataBaseItems();
	CString GetComment();
	BOOL GetDisallowImport();
	CString GetName();
	CString GetNameWithPath();
	CString GetOID();
	LPDISPATCH GetParentFolder();
	CString GetType();
	BOOL IsAsap2CanDB();
	BOOL IsAsap2Catalog();
	BOOL IsAsap2Item();
	BOOL IsAsap2Project();
	BOOL IsAutosarFolder();
	BOOL IsAsap2ProjectFolder();
	BOOL IsAscetContainer();
	BOOL IsAscetFolder();
	BOOL IsAscetNetwork();
	BOOL IsAscetProject();
	BOOL IsAscetProtocol();
	BOOL IsClass();
	BOOL IsCodeComponent();
	BOOL IsComponent();
	BOOL IsConditionalTable();
	BOOL IsCTBlock();
	BOOL IsCTComponent();
	BOOL IsDataBaseItem();
	BOOL IsDataSet();
	BOOL IsDiscreteComponent();
	BOOL IsEnumeration();
	BOOL IsExperimentEnvironment();
	BOOL IsODXProject();
	BOOL IsFunctionalComponent();
	BOOL IsHardwareConfiguration();
	BOOL IsIcon();
	BOOL IsModule();
	BOOL IsProject();
	BOOL IsSignal();
	BOOL IsStateMachine();
	BOOL Move(LPDISPATCH dispatch_folder);
	BOOL SetAccessRightApplication(BOOL aBool);
	BOOL SetAccessRightCodeGeneration(BOOL aBool);
	BOOL SetAccessRightExecute(BOOL aBool);
	BOOL SetAccessRightRead(BOOL aBool);
	BOOL SetAccessRightWrite(BOOL aBool);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetDisallowImport(BOOL aBool);
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// IncaOnlineExperiment_Dispatch wrapper class

class IncaOnlineExperiment_Dispatch : public COleDispatchDriver
{
public:
	IncaOnlineExperiment_Dispatch() {}		// Calls COleDispatchDriver default constructor
	IncaOnlineExperiment_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IncaOnlineExperiment_Dispatch(const IncaOnlineExperiment_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetEtas_binary();
	CString GetDiadem_atf();
	CString GetEtas_ascii();
	CString GetEtas_groupascii();
	CString GetMatlab_mfile();
	CString GetFamos_record();
	CString GetEtas_mdf();
	CString GetEtas_mdf4();
	BOOL ChangeWorkingData(LPCTSTR aDataSetName);
	BOOL CheckRecordingTriggerFormula(LPCTSTR triggerString);
	BOOL ClearRecordingStateOf(LPDISPATCH dispatch_aMeasureDataItem);
	BOOL GetAcquisitionState();
	long GetActualRecordingTimeMs();
	VARIANT GetAllActiveMeasureLabels();
	VARIANT GetAllPossibleTriggerOperators();
	long GetGlobalMeasureStateIndex();
	CString GetGlobalMeasureStateLabel();
	long GetLastTSMessageCategoryBuffered(BOOL aBool);
	CString GetLastTSMessageStringBuffered(BOOL aBool);
	BOOL GetRecordingFileAutoincrementFlag();
	CString GetRecordingFileFormat();
	VARIANT GetRecordingFileFormats();
	VARIANT GetRecordingFileFormatsInternal();
	CString GetPrimaryRecordingFormatID();
	CString GetPrimaryRecordingFormatFileExtension();
	CString GetRecordingFileName();
	CString GetLastRecordingFileName();
	CString GetRecordingHeaderComment();
	CString GetRecordingHeaderCompany();
	CString GetRecordingHeaderDefaultComment();
	CString GetRecordingHeaderProject();
	CString GetRecordingHeaderUser();
	CString GetRecordingHeaderVehicle();
	CString GetRecordingPathName();
	long GetRemainingRecordingTimeMs();
	CString GetStateOf(LPDISPATCH dispatch_anExperimentDataItem);
	BOOL InitializeHardware();
	BOOL InitializeHardwareWithECUReset();
	BOOL IsIncaOnlineExperiment();
	BOOL IsMeasureLabelActive(LPCTSTR measureLabel, LPDISPATCH dispatch_device);
	double RecordingTimeActual();
	double RecordingTimeRemaining();
	BOOL RecordWithDuration(double duration);
	BOOL RecordWithManualStartPreTimeManualStop(double preTriggerTime);
	BOOL RecordWithManualStartPreTimePostTime(double preTriggerTime, double postTriggerTime);
	BOOL RecordWithManualStartPreTimeStopTrigger(double preTriggerTime, LPCTSTR stopTriggerString);
	BOOL RecordWithManualStop();
	BOOL RecordWithManuelStop();
	BOOL RecordWithStartTriggerDuration(LPCTSTR startTriggerString, double duration);
	BOOL RecordWithStartTriggerManualStop(LPCTSTR startTriggerString);
	BOOL RecordWithStartTriggerPreTimeManualStop(LPCTSTR startTriggerString, double preTriggerTime);
	BOOL RecordWithStartTriggerPreTimePostTime(LPCTSTR startTriggerString, double preTriggerTime, double postTriggerTime);
	BOOL RecordWithStartTriggerPreTimeStopTrigger(LPCTSTR startTriggerString, double preTriggerTime, LPCTSTR stopTriggerString);
	BOOL SendManualStartTrigger();
	BOOL SendManualStopTrigger();
	BOOL SetLastRecordingFileName(LPCTSTR fileName);
	BOOL SetRecordingFileAttributes(LPCTSTR fileName, LPCTSTR pathName, LPCTSTR formatName, BOOL state);
	BOOL SetRecordingFileAutoincrementFlag(BOOL state);
	BOOL DisableRecordingFileDateTimeSuffix();
	long GetRecordingRepetitionNumber();
	BOOL SetRecordingRepetitionNumber(long aNumber);
	BOOL SetRecordingFileFormat(LPCTSTR formatName);
	BOOL SetRecordingFileName(LPCTSTR fileName);
	BOOL SetRecordingHeader(LPCTSTR comment, LPCTSTR user, LPCTSTR company, LPCTSTR project, LPCTSTR vehicle);
	BOOL SetRecordingHeaderComment(LPCTSTR comment);
	BOOL SetRecordingHeaderCompany(LPCTSTR company);
	BOOL SetRecordingHeaderProject(LPCTSTR project);
	BOOL SetRecordingHeaderUser(LPCTSTR user);
	BOOL SetRecordingHeaderVehicle(LPCTSTR vehicle);
	BOOL SetRecordingPathName(LPCTSTR pathName);
	BOOL SetRecordingStateOf(LPDISPATCH dispatch_aMeasureDataItem);
	BOOL SetStateFor(LPCTSTR state, LPDISPATCH dispatch_anExperimentDataItem);
	BOOL SwitchCalibrationAccessOff();
	BOOL SwitchCalibrationAccessOn();
	BOOL EnableEtherCATAccess(LPCTSTR aLabel, LPDISPATCH dispatch_device);
	BOOL DisableEtherCATAccess(LPCTSTR aLabel, LPDISPATCH dispatch_device);
	BOOL EnableFULIAccess(LPCTSTR aLabel, LPDISPATCH dispatch_device);
	BOOL DisableFULIAccess(LPCTSTR aLabel, LPDISPATCH dispatch_device);
	CString GetAsap2NameForDisplayIdentifier(LPCTSTR aDisplayIdentifier, LPDISPATCH dispatch_aDevice);
	BOOL WriteEventComment(LPCTSTR aComment, BOOL aFlag);
	VARIANT GetScenarioNames();
	BOOL ActivateScenario(LPCTSTR scenarioName);
	LPDISPATCH GetMCD3Extensions(LPCTSTR AccessControlModifierKey_ETASEngineering);
	BOOL IsOdxProjectLoaded();
	LPDISPATCH GetDiagnosticExtensionsForLoadedODXProject();
	VARIANT GetOpenLogicalLinks();
	VARIANT GetAllExperimentSolverBlockLabels();
	VARIANT GetAllSolverIntegratorLabels();
	CString GetIntegratorNameForSolver(LPCTSTR aLabel);
	double GetIntegratorParameterForSolver(LPCTSTR parameterLabel, LPCTSTR aLabel);
	VARIANT GetIntegratorParameterLabels();
	BOOL IsExperimentLocked();
	BOOL IsMeasurementRunning();
	BOOL IsOnlineExperiment();
	BOOL LockExperiment();
	BOOL SetIntegratorAdamsMoulton2ForSolver(LPCTSTR aLabel, double dt, double h);
	BOOL SetIntegratorEulerForSolver(LPCTSTR aLabel, double dt, double h);
	BOOL SetIntegratorHeunForSolver(LPCTSTR aLabel, double dt, double h);
	BOOL SetIntegratorMulstep2ForSolver(LPCTSTR aLabel, double dt, double h);
	BOOL SetIntegratorParameterForSolver(LPCTSTR parameterLabel, LPCTSTR aLabel, double value_Renamed);
	BOOL SetIntegratorRungeKutta4ForSolver(LPCTSTR aLabel, double dt, double h);
	BOOL StartMeasurement();
	BOOL StopMeasurement();
	BOOL StopMeasurementAndSaveToFile(LPCTSTR fileName, LPCTSTR fileFormat);
	BOOL UnlockExperiment();
	VARIANT BrowseCalibrationElement(LPCTSTR name);
	VARIANT BrowseCalibrationElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseCaseInsensitiveCalibrationElement(LPCTSTR name);
	VARIANT BrowseCaseInsensitiveCalibrationElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseCaseInsensitiveExperimentElement(LPCTSTR name);
	VARIANT BrowseCaseInsensitiveExperimentElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseCaseInsensitiveMeasureElement(LPCTSTR name);
	VARIANT BrowseCaseInsensitiveMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseExperimentElement(LPCTSTR name);
	VARIANT BrowseExperimentElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseMeasureElement(LPCTSTR name);
	VARIANT BrowseMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	long CheckTypeForElementNameInDevice(LPCTSTR aLabelName, LPDISPATCH dispatch_aDevice);
	VARIANT GetAllCalibrationElements();
	VARIANT GetAllCalibrationElementsInDevice(LPDISPATCH dispatch_device);
	VARIANT GetAllDevices();
	VARIANT GetAllExperimentElements();
	VARIANT GetAllExperimentElementsInDevice(LPDISPATCH dispatch_device);
	VARIANT GetAllMeasureElements();
	VARIANT GetAllMeasureElementsInDevice(LPDISPATCH dispatch_device);
	LPDISPATCH GetCalibrationElement(LPCTSTR name);
	LPDISPATCH GetCalibrationElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetCalibrationValue(LPCTSTR name);
	LPDISPATCH GetCalibrationValueInDevice(LPCTSTR name, LPDISPATCH dispatch_dev);
	CString GetDefaultRasterForMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetDevice(LPCTSTR name);
	LPDISPATCH GetExperimentElement(LPCTSTR name);
	LPDISPATCH GetExperimentElementForCaseInsensitiveLabel(LPCTSTR name);
	LPDISPATCH GetExperimentElementForCaseInsensitiveLabelInDevice(LPCTSTR name, LPDISPATCH dispatch_dev);
	LPDISPATCH GetExperimentElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetMeasureElement(LPCTSTR name);
	LPDISPATCH GetMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetMeasureValue(LPCTSTR name);
	LPDISPATCH GetMeasureValueInDevice(LPCTSTR name, LPDISPATCH dispatch_dev);
	LPDISPATCH GetMeasureValueWithAcquisitionRate(LPCTSTR name, LPCTSTR acquisitionRate);
	LPDISPATCH GetMeasureValueWithAcquisitionRateInDevice(LPCTSTR name, LPCTSTR acquisitionRate, LPDISPATCH dispatch_dev);
	VARIANT GetMeasurementsWithRCI2(const VARIANT& devNames, const VARIANT& rasterNames, const VARIANT& messNames);
	BOOL GetRecordingState();
	BOOL IsExperiment();
	BOOL PauseRecording();
	BOOL StartRecording();
	BOOL StopAndDiscardRecording();
	BOOL StopRecording(LPCTSTR fileName, LPCTSTR fileFormat);
	BOOL ConvertMdfFile(LPCTSTR fileName, LPCTSTR fileFormat);
	VARIANT GetHWDevicesFlat(LPDISPATCH dispatch_aHWConfig);
	VARIANT GetDependentCommonAxisConsumers(LPCTSTR elementName);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// KWP2000Device_Dispatch wrapper class

class KWP2000Device_Dispatch : public COleDispatchDriver
{
public:
	KWP2000Device_Dispatch() {}		// Calls COleDispatchDriver default constructor
	KWP2000Device_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	KWP2000Device_Dispatch(const KWP2000Device_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT ClearDiagnosticInformation(long groupOfDiagnosticInformation);
	VARIANT DynamicallyDefineLocalIdentifier(const VARIANT& dataArray);
	VARIANT EcuReset(long resetStatus);
	VARIANT EscapeCode(long manufacturerSpecific, const VARIANT& recordValue);
	BOOL FinishDirectDiagnosticAccess();
	VARIANT GetCommParas(long paramType);
	VARIANT GetEcuIdent(long indentNo);
	VARIANT IdentifyEcu(long identFormat);
	BOOL InitDirectDiagnosticAccess();
	VARIANT InputOutputControlByCommonIdentifier(long inputOutputCommonIdentifier, const VARIANT& controlOption);
	VARIANT InputOutputControlByLocalIdentifier(long inputOutputLocalIdentifier, const VARIANT& controlOption);
	BOOL IsKwp2000Device();
	VARIANT ReadDataByCommonIdentifier(long recordCommonIdentifier);
	VARIANT ReadDataByLocalIdentifier(long recordLocalIdentifier);
	VARIANT ReadDiasgnosticTroubleCodesByStatus(long statusOfDTC, long groupOfDTC);
	VARIANT ReadECUIdentification(long identificationOption);
	VARIANT ReadFreezeFrameData(long freezeFrameNumber, long recordAccess, long recordIdentification);
	VARIANT ReadMemoryByAddress(long memoryAddress, long memorySize);
	VARIANT ReadStatusOfDiasgnosticTroubleCodes(long groupOfDTC);
	VARIANT RequestDownload(long memoryAddress, long dataFormatIdentifier, long memorySize);
	VARIANT RequestRoutineResutsByAddress(long routineAddress);
	VARIANT RequestRoutineResutsByLocalIdentifier(long routineLocalIdentifier);
	VARIANT RequestTransferExit(const VARIANT& transferRequestParameter);
	VARIANT RequestUpload(long memoryAddress, long dataFormatIdentifier, long memorySize);
	VARIANT SecurityAccessAllowed(long accessMode);
	VARIANT SecurityAccessAllowedUsingKey(long accessMode, const VARIANT& key);
	VARIANT SendFrameWithByteArray(const VARIANT& data);
	VARIANT SendFrameWithShortArray(const VARIANT& data);
	VARIANT SendMessageWithByteArray(const VARIANT& data);
	VARIANT SendMessageWithShortArray(const VARIANT& data);
	BOOL SetCommParas(long p1Max, long p2Min, long p2Max, long p3Min, long p3Max, long p4Min);
	BOOL SetCyclicDiagnosticMessageWithByteArray(long mode, long delay, const VARIANT& data);
	BOOL SetCyclicDiagnosticMessageWithShortArray(long mode, long delay, const VARIANT& data);
	BOOL SetDefaultCommParas();
	BOOL SetDiagnosticIfAnalogOutput(long voltage);
	BOOL SetDiagnosticIfBaudrate(long baudrate);
	BOOL SetDiagnosticIfRepeatCounter(long counter);
	BOOL SetDiagnosticIfSpecialCond(long specialCond);
	BOOL SetDiagnosticIfTiming(long p1Max, long p2Min, long p2Max, long p3Min, long p3Max, long p4Min);
	VARIANT StartDiagnosticCommunication(long protocolType, const VARIANT& timingParameter, long baud, long w1, long w2, long w3, long w4Min, long w4Max, long w5, long tiniL, long wuP, long activationLine, long activationType, long target, 
		long source);
	VARIANT StartDiagnosticSession(long diagnosticMode, long baudrateIdentifier);
	VARIANT StartDiagnosticSession115200Baud(long diagnosticMode);
	VARIANT StartDiagnosticSession19200Baud(long diagnosticMode);
	VARIANT StartDiagnosticSession38400Baud(long diagnosticMode);
	VARIANT StartDiagnosticSession57600Baud(long diagnosticMode);
	VARIANT StartDiagnosticSession9600Baud(long diagnosticMode);
	VARIANT StartDiagnosticSessionUserDefinedBaud(long diagnosticMode, long baudrate);
	VARIANT StartRoutineByAddress(long routineAddress, const VARIANT& routineEntryOption);
	VARIANT StartRoutineByLocalIdentifier(long routineLocalIdentifier, const VARIANT& routineEntryOption);
	VARIANT StopDiagnosticSession();
	VARIANT StopRoutineByAddress(long routineAddress, const VARIANT& routineExitOption);
	VARIANT StopRoutineByLocalIdentifier(long routineLocalIdentifier, const VARIANT& routineExitOption);
	VARIANT TesterPresent();
	VARIANT TransferData(const VARIANT& transferRequestParameter);
	VARIANT WriteDataByCommonIdentifier(long recordCommonIdentifier, const VARIANT& memData);
	VARIANT WriteDataByLocalIdentifier(long recordLocalIdentifier, const VARIANT& memData);
	VARIANT WriteMemoryByAddress(long memoryAddress, const VARIANT& memData);
	BOOL CheckCodePageConform();
	BOOL DoChecksumCalculation();
	BOOL CheckDataPagesConform();
	BOOL IsCalibrationPossible();
	BOOL IsTargetEcuOff();
	BOOL CanSwitchMemoryPage();
	BOOL CopyBinaryToFile(LPCTSTR aPathAndFileName);
	BOOL CopyReferencePageToWorkingPage();
	BOOL CopyReferencePageToETKFlash();
	BOOL CopyWorkingPageToETKFlash();
	BOOL CopyReferencePageToFlash(BOOL flagForDataOnly);
	BOOL CopyWorkingPageToFlash(BOOL flagForDataOnly);
	BOOL CreateHexFileForReferencePageAndCode(LPCTSTR fileName);
	BOOL CreateHexFileForWorkPageAndCode(LPCTSTR fileName);
	BOOL DisableDownloadDifferencesAfterIgnitionOffOn();
	BOOL DownloadDifferences();
	BOOL DownloadPage(LPCTSTR name);
	BOOL DownloadReferencePage();
	BOOL DownloadReferencePageAndCode();
	BOOL DownloadWorkbase();
	BOOL DownloadWorkPage();
	BOOL DownloadWorkPageAndCode();
	BOOL EnableDownloadDifferencesAfterIgnitionOffOn();
	BOOL FlashHexFileWithControlFlow(LPCTSTR fileName, LPCTSTR controlFlow);
	BOOL ExecuteODXFlashing(LPCTSTR hexFileName, LPCTSTR projectName, LPCTSTR vehicleInfo, LPCTSTR logicalLink, LPCTSTR flashSession, BOOL automaticVariantIdentification);
	BOOL FlashHexFileWithControlFlowOnFlashPort(LPCTSTR fileName, LPCTSTR controlFlow, LPCTSTR flashPort);
	VARIANT GetAllPageNames();
	CString GetBinaryFileName();
	long GetCodeCheckStatus();
	VARIANT GetDataAddressRanges();
	VARIANT GetDataAddressRangesAsHexString();
	CString GetDescriptionFileName();
	CString GetEPKfromASAP2();
	CString GetEPKfromDS();
	CString GetEPKfromECU();
	VARIANT GetExRAMAddressRanges();
	CString GetReferenceDataSetDataBasePath();
	VARIANT GetSupportedFlashPortNames();
	CString GetWorkingDataSetDataBasePath();
	BOOL IsConfiguredForOdxFlashing();
	BOOL IsCurrentPageWriteProtected();
	BOOL IsTargetOnWorkPage();
	BOOL IsWorkbaseDevice();
	BOOL ReadBinaryFromFile(LPCTSTR aPathOrFileName, BOOL createNewDS, BOOL download);
	BOOL ReadAllCalibrationsFromFile(LPCTSTR aPathOrFileName);
	VARIANT ReadFromDataPage(long start, long length);
	VARIANT ReadFromExRAM(long start, long length);
	BOOL SetOrCreateWorkBase(LPCTSTR aPathOrFileName, BOOL aFlag);
	BOOL SetWorkingDataSet(LPDISPATCH dispatch_aDataSet);
	BOOL SwitchToReferencePage();
	BOOL SwitchToWorkingPage();
	BOOL UploadWorkbase();
	BOOL UploadWorkbaseUsePageNames(LPCTSTR referencePageName, LPCTSTR workingPageName);
	BOOL UploadWorkbaseUsePageNamesConditional(LPCTSTR rpName, LPCTSTR wpName, long rpCondition, long wpCondition);
	BOOL WriteToDataPage(long start, long length, const VARIANT& values);
	BOOL WriteToExRAM(long start, long length, const VARIANT& values);
	BOOL InCircuit2ExperimentPreparation(const VARIANT& emulatedLabelNames, LPCTSTR controlFlow);
	BOOL SetStoreBit();
	BOOL RemoveStoreBit();
	BOOL QuickStart_IsFeatureInstalled();
	BOOL QuickStart_IsQuickStartModeConfiguredInHWC();
	BOOL QuickStart_StoreMeasurementConfigurationInECU();
	BOOL QuickStart_DeleteMeasurementConfigurationInECU();
	BOOL QuickStart_Enable();
	BOOL QuickStart_Disable();
	VARIANT ActivateRci2ForAcquisitionRate(LPCTSTR name);
	BOOL CanAssignProject();
	BOOL DeActivateRci2ForAcquisitionRate(LPCTSTR name);
	double GetA2lScalingFactorOfSignalGroup(LPCTSTR signalGroupName);
	long GetA2lScalingUnitOfSignalGroup(LPCTSTR signalGroupName);
	VARIANT GetAllAcquisitionRates();
	LPDISPATCH GetASAP2Module();
	CString GetDeviceType();
	CString GetName();
	CString GetProjectDataBasePath();
	double GetSampleRateFor(LPCTSTR aName);
	BOOL IsAcquisitionRateAvailable(LPCTSTR rate);
	BOOL SetAsyncBuffersToInvalid();
	BOOL IsActive();
	BOOL IsCANMonitoringDevice();
	BOOL IsExperimentDevice();
	BOOL IsOutputDevice();
	BOOL IsEtherCATSupported();
	BOOL IsFULISupported();
	BOOL ResetTarget();
	BOOL StartTargetOS();
	BOOL StopTargetOS();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// KWP2000Response_Dispatch wrapper class

class KWP2000Response_Dispatch : public COleDispatchDriver
{
public:
	KWP2000Response_Dispatch() {}		// Calls COleDispatchDriver default constructor
	KWP2000Response_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	KWP2000Response_Dispatch(const KWP2000Response_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString ErrorText();
	BOOL IsBusy();
	BOOL IsError();
	long ServiceId();
};
/////////////////////////////////////////////////////////////////////////////
// WorkbaseDevice_Dispatch wrapper class

class WorkbaseDevice_Dispatch : public COleDispatchDriver
{
public:
	WorkbaseDevice_Dispatch() {}		// Calls COleDispatchDriver default constructor
	WorkbaseDevice_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	WorkbaseDevice_Dispatch(const WorkbaseDevice_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL CheckCodePageConform();
	BOOL DoChecksumCalculation();
	BOOL CheckDataPagesConform();
	BOOL IsCalibrationPossible();
	BOOL IsTargetEcuOff();
	BOOL CanSwitchMemoryPage();
	BOOL CopyBinaryToFile(LPCTSTR aPathAndFileName);
	BOOL CopyReferencePageToWorkingPage();
	BOOL CopyReferencePageToETKFlash();
	BOOL CopyWorkingPageToETKFlash();
	BOOL CopyReferencePageToFlash(BOOL flagForDataOnly);
	BOOL CopyWorkingPageToFlash(BOOL flagForDataOnly);
	BOOL CreateHexFileForReferencePageAndCode(LPCTSTR fileName);
	BOOL CreateHexFileForWorkPageAndCode(LPCTSTR fileName);
	BOOL DisableDownloadDifferencesAfterIgnitionOffOn();
	BOOL DownloadDifferences();
	BOOL DownloadPage(LPCTSTR name);
	BOOL DownloadReferencePage();
	BOOL DownloadReferencePageAndCode();
	BOOL DownloadWorkbase();
	BOOL DownloadWorkPage();
	BOOL DownloadWorkPageAndCode();
	BOOL EnableDownloadDifferencesAfterIgnitionOffOn();
	BOOL FlashHexFileWithControlFlow(LPCTSTR fileName, LPCTSTR controlFlow);
	BOOL ExecuteODXFlashing(LPCTSTR hexFileName, LPCTSTR projectName, LPCTSTR vehicleInfo, LPCTSTR logicalLink, LPCTSTR flashSession, BOOL automaticVariantIdentification);
	BOOL FlashHexFileWithControlFlowOnFlashPort(LPCTSTR fileName, LPCTSTR controlFlow, LPCTSTR flashPort);
	VARIANT GetAllPageNames();
	CString GetBinaryFileName();
	long GetCodeCheckStatus();
	VARIANT GetDataAddressRanges();
	VARIANT GetDataAddressRangesAsHexString();
	CString GetDescriptionFileName();
	CString GetEPKfromASAP2();
	CString GetEPKfromDS();
	CString GetEPKfromECU();
	VARIANT GetExRAMAddressRanges();
	CString GetReferenceDataSetDataBasePath();
	VARIANT GetSupportedFlashPortNames();
	CString GetWorkingDataSetDataBasePath();
	BOOL IsConfiguredForOdxFlashing();
	BOOL IsCurrentPageWriteProtected();
	BOOL IsTargetOnWorkPage();
	BOOL IsWorkbaseDevice();
	BOOL ReadBinaryFromFile(LPCTSTR aPathOrFileName, BOOL createNewDS, BOOL download);
	BOOL ReadAllCalibrationsFromFile(LPCTSTR aPathOrFileName);
	VARIANT ReadFromDataPage(long start, long length);
	VARIANT ReadFromExRAM(long start, long length);
	BOOL SetOrCreateWorkBase(LPCTSTR aPathOrFileName, BOOL aFlag);
	BOOL SetWorkingDataSet(LPDISPATCH dispatch_aDataSet);
	BOOL SwitchToReferencePage();
	BOOL SwitchToWorkingPage();
	BOOL UploadWorkbase();
	BOOL UploadWorkbaseUsePageNames(LPCTSTR referencePageName, LPCTSTR workingPageName);
	BOOL UploadWorkbaseUsePageNamesConditional(LPCTSTR rpName, LPCTSTR wpName, long rpCondition, long wpCondition);
	BOOL WriteToDataPage(long start, long length, const VARIANT& values);
	BOOL WriteToExRAM(long start, long length, const VARIANT& values);
	BOOL InCircuit2ExperimentPreparation(const VARIANT& emulatedLabelNames, LPCTSTR controlFlow);
	BOOL SetStoreBit();
	BOOL RemoveStoreBit();
	BOOL QuickStart_IsFeatureInstalled();
	BOOL QuickStart_IsQuickStartModeConfiguredInHWC();
	BOOL QuickStart_StoreMeasurementConfigurationInECU();
	BOOL QuickStart_DeleteMeasurementConfigurationInECU();
	BOOL QuickStart_Enable();
	BOOL QuickStart_Disable();
	VARIANT ActivateRci2ForAcquisitionRate(LPCTSTR name);
	BOOL CanAssignProject();
	BOOL DeActivateRci2ForAcquisitionRate(LPCTSTR name);
	double GetA2lScalingFactorOfSignalGroup(LPCTSTR signalGroupName);
	long GetA2lScalingUnitOfSignalGroup(LPCTSTR signalGroupName);
	VARIANT GetAllAcquisitionRates();
	LPDISPATCH GetASAP2Module();
	CString GetDeviceType();
	CString GetName();
	CString GetProjectDataBasePath();
	double GetSampleRateFor(LPCTSTR aName);
	BOOL IsAcquisitionRateAvailable(LPCTSTR rate);
	BOOL SetAsyncBuffersToInvalid();
	BOOL IsActive();
	BOOL IsCANMonitoringDevice();
	BOOL IsExperimentDevice();
	BOOL IsKwp2000Device();
	BOOL IsOutputDevice();
	BOOL IsEtherCATSupported();
	BOOL IsFULISupported();
	BOOL ResetTarget();
	BOOL StartTargetOS();
	BOOL StopTargetOS();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationCube3Data_Dispatch wrapper class

class CalibrationCube3Data_Dispatch : public COleDispatchDriver
{
public:
	CalibrationCube3Data_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationCube3Data_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationCube3Data_Dispatch(const CalibrationCube3Data_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetBooleanPhysValue();
	VARIANT GetDoublePhysValue();
	VARIANT GetFloatPhysValue();
	VARIANT GetImplValue();
	VARIANT GetIntegerPhysValue();
	VARIANT GetLongPhysValue();
	VARIANT GetDoublePhysValuesRange(long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	VARIANT GetFloatPhysValuesRange(long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	VARIANT GetDoubleImplValuesRange(long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	VARIANT GetFloatImplValuesRange(long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	VARIANT GetBooleanImplValuesRange(long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	VARIANT GetIntegerImplValuesRange(long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	VARIANT GetLongImplValuesRange(long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	CString GetPhysType();
	CString GetImplType();
	long GetXSize();
	long GetYSize();
	long GetZSize();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL SetDoublePhysValue(const VARIANT& valCube3);
	BOOL SetBooleanPhysValue(const VARIANT& valCube3);
	BOOL SetFloatPhysValue(const VARIANT& valCube3);
	BOOL SetImplValue(const VARIANT& valCube3);
	BOOL SetIntegerPhysValue(const VARIANT& valCube3);
	BOOL SetLongPhysValue(const VARIANT& valCube3);
	BOOL SetDoublePhysValuesRange(const VARIANT& valCube3, long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	BOOL SetFloatPhysValuesRange(const VARIANT& valCube3, long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	BOOL SetDoubleImplValuesRange(const VARIANT& valCube3, long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	BOOL SetFloatImplValuesRange(const VARIANT& valCube3, long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	BOOL SetBooleanImplValuesRange(const VARIANT& valCube3, long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	BOOL SetIntegerImplValuesRange(const VARIANT& valCube3, long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	BOOL SetLongImplValuesRange(const VARIANT& valCube3, long startXIndex, long startYIndex, long startZIndex, long endXIndex, long endYIndex, long endZIndex);
	BOOL SetXSize(long xSize);
	BOOL SetYSize(long ySize);
	BOOL SetZSize(long zSize);
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationCube4Data_Dispatch wrapper class

class CalibrationCube4Data_Dispatch : public COleDispatchDriver
{
public:
	CalibrationCube4Data_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationCube4Data_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationCube4Data_Dispatch(const CalibrationCube4Data_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetBooleanPhysValue();
	VARIANT GetDoublePhysValue();
	VARIANT GetFloatPhysValue();
	VARIANT GetImplValue();
	VARIANT GetIntegerPhysValue();
	VARIANT GetLongPhysValue();
	VARIANT GetDoublePhysValuesRange(long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	VARIANT GetFloatPhysValuesRange(long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	VARIANT GetDoubleImplValuesRange(long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	VARIANT GetFloatImplValuesRange(long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	VARIANT GetBooleanImplValuesRange(long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	VARIANT GetIntegerImplValuesRange(long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	VARIANT GetLongImplValuesRange(long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	CString GetPhysType();
	CString GetImplType();
	long GetXSize();
	long GetYSize();
	long GetZSize();
	long GetZ2Size();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL SetDoublePhysValue(const VARIANT& valCube4);
	BOOL SetBooleanPhysValue(const VARIANT& valCube4);
	BOOL SetFloatPhysValue(const VARIANT& valCube4);
	BOOL SetImplValue(const VARIANT& valCube4);
	BOOL SetIntegerPhysValue(const VARIANT& valCube4);
	BOOL SetLongPhysValue(const VARIANT& valCube4);
	BOOL SetDoublePhysValuesRange(const VARIANT& valCube4, long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	BOOL SetFloatPhysValuesRange(const VARIANT& valCube4, long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	BOOL SetDoubleImplValuesRange(const VARIANT& valCube4, long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	BOOL SetFloatImplValuesRange(const VARIANT& valCube4, long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	BOOL SetBooleanImplValuesRange(const VARIANT& valCube4, long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	BOOL SetIntegerImplValuesRange(const VARIANT& valCube4, long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	BOOL SetLongImplValuesRange(const VARIANT& valCube4, long startXIndex, long startYIndex, long startZIndex, long startZ2Index, long endXIndex, long endYIndex, long endZIndex, long endZ2Index);
	BOOL SetXSize(long xSize);
	BOOL SetYSize(long ySize);
	BOOL SetZSize(long zSize);
	BOOL SetZ2Size(long z2Size);
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationFourDTableData_Dispatch wrapper class

class CalibrationFourDTableData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationFourDTableData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationFourDTableData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationFourDTableData_Dispatch(const CalibrationFourDTableData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetValue();
	LPDISPATCH GetXDistribution();
	LPDISPATCH GetYDistribution();
	LPDISPATCH GetZDistribution();
	LPDISPATCH GetZ2Distribution();
	BOOL IsFourDTable();
	BOOL SetValue(LPDISPATCH dispatch_cube4);
	BOOL SetXDistribution(LPDISPATCH dispatch_dis);
	BOOL SetYDistribution(LPDISPATCH dispatch_dis);
	BOOL SetZDistribution(LPDISPATCH dispatch_dis);
	BOOL SetZ2Distribution(LPDISPATCH dispatch_dis);
	double GetCalibrationImplZ2AxisMinIncrement();
	double GetCalibrationImplZ2AxisWeakBoundMax();
	double GetCalibrationImplZ2AxisWeakBoundMin();
	double GetCalibrationZ2AxisMinIncrement();
	CString GetCalibrationZ2AxisSourceLabel();
	double GetCalibrationZ2AxisWeakBoundMax();
	double GetCalibrationZ2AxisWeakBoundMin();
	BOOL SetZ2AxisWeakBoundtoHardBound();
	double GetCalibrationImplZAxisMinIncrement();
	double GetCalibrationImplZAxisWeakBoundMax();
	double GetCalibrationImplZAxisWeakBoundMin();
	double GetCalibrationZAxisMinIncrement();
	CString GetCalibrationZAxisSourceLabel();
	double GetCalibrationZAxisWeakBoundMax();
	double GetCalibrationZAxisWeakBoundMin();
	BOOL SetZAxisWeakBoundtoHardBound();
	double GetCalibrationImplYAxisMinIncrement();
	double GetCalibrationImplYAxisWeakBoundMax();
	double GetCalibrationImplYAxisWeakBoundMin();
	double GetCalibrationYAxisMinIncrement();
	CString GetCalibrationYAxisSourceLabel();
	double GetCalibrationYAxisWeakBoundMax();
	double GetCalibrationYAxisWeakBoundMin();
	BOOL SetYAxisWeakBoundtoHardBound();
	double GetCalibrationImplXAxisMinIncrement();
	double GetCalibrationImplXAxisWeakBoundMax();
	double GetCalibrationImplXAxisWeakBoundMin();
	double GetCalibrationXAxisMinIncrement();
	CString GetCalibrationXAxisSourceLabel();
	double GetCalibrationXAxisWeakBoundMax();
	double GetCalibrationXAxisWeakBoundMin();
	BOOL SetXAxisWeakBoundtoHardBound();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationThreeDTableData_Dispatch wrapper class

class CalibrationThreeDTableData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationThreeDTableData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationThreeDTableData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationThreeDTableData_Dispatch(const CalibrationThreeDTableData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetValue();
	LPDISPATCH GetXDistribution();
	LPDISPATCH GetYDistribution();
	LPDISPATCH GetZDistribution();
	BOOL IsThreeDTable();
	BOOL SetValue(LPDISPATCH dispatch_cube3);
	BOOL SetXDistribution(LPDISPATCH dispatch_dis);
	BOOL SetYDistribution(LPDISPATCH dispatch_dis);
	BOOL SetZDistribution(LPDISPATCH dispatch_dis);
	double GetCalibrationImplZAxisMinIncrement();
	double GetCalibrationImplZAxisWeakBoundMax();
	double GetCalibrationImplZAxisWeakBoundMin();
	double GetCalibrationZAxisMinIncrement();
	CString GetCalibrationZAxisSourceLabel();
	double GetCalibrationZAxisWeakBoundMax();
	double GetCalibrationZAxisWeakBoundMin();
	BOOL SetZAxisWeakBoundtoHardBound();
	double GetCalibrationImplYAxisMinIncrement();
	double GetCalibrationImplYAxisWeakBoundMax();
	double GetCalibrationImplYAxisWeakBoundMin();
	double GetCalibrationYAxisMinIncrement();
	CString GetCalibrationYAxisSourceLabel();
	double GetCalibrationYAxisWeakBoundMax();
	double GetCalibrationYAxisWeakBoundMin();
	BOOL SetYAxisWeakBoundtoHardBound();
	double GetCalibrationImplXAxisMinIncrement();
	double GetCalibrationImplXAxisWeakBoundMax();
	double GetCalibrationImplXAxisWeakBoundMin();
	double GetCalibrationXAxisMinIncrement();
	CString GetCalibrationXAxisSourceLabel();
	double GetCalibrationXAxisWeakBoundMax();
	double GetCalibrationXAxisWeakBoundMin();
	BOOL SetXAxisWeakBoundtoHardBound();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Cube3CalibrationElement_Dispatch wrapper class

class Cube3CalibrationElement_Dispatch : public COleDispatchDriver
{
public:
	Cube3CalibrationElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Cube3CalibrationElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Cube3CalibrationElement_Dispatch(const Cube3CalibrationElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsCube3();
	BOOL IsCalibrationElement();
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsMeasureElement();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Cube4CalibrationElement_Dispatch wrapper class

class Cube4CalibrationElement_Dispatch : public COleDispatchDriver
{
public:
	Cube4CalibrationElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Cube4CalibrationElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Cube4CalibrationElement_Dispatch(const Cube4CalibrationElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsCube4();
	BOOL IsCalibrationElement();
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsMeasureElement();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// FourDTableCalibrationElement_Dispatch wrapper class

class FourDTableCalibrationElement_Dispatch : public COleDispatchDriver
{
public:
	FourDTableCalibrationElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	FourDTableCalibrationElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	FourDTableCalibrationElement_Dispatch(const FourDTableCalibrationElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsFourDTable();
	BOOL IsCalibrationElement();
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsMeasureElement();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// ThreeDTableCalibrationElement_Dispatch wrapper class

class ThreeDTableCalibrationElement_Dispatch : public COleDispatchDriver
{
public:
	ThreeDTableCalibrationElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	ThreeDTableCalibrationElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ThreeDTableCalibrationElement_Dispatch(const ThreeDTableCalibrationElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsThreeDTable();
	BOOL IsCalibrationElement();
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsMeasureElement();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// ArrayCalibrationElement_Dispatch wrapper class

class ArrayCalibrationElement_Dispatch : public COleDispatchDriver
{
public:
	ArrayCalibrationElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	ArrayCalibrationElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ArrayCalibrationElement_Dispatch(const ArrayCalibrationElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsArray();
	BOOL IsCalibrationElement();
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsMeasureElement();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// ArrayMeasureElement_Dispatch wrapper class

class ArrayMeasureElement_Dispatch : public COleDispatchDriver
{
public:
	ArrayMeasureElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	ArrayMeasureElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ArrayMeasureElement_Dispatch(const ArrayMeasureElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsArray();
	LPDISPATCH GetValueForAcquisitionRate(LPCTSTR rate);
	BOOL IsMeasureElement();
	BOOL OpenViewWithMaxLabels(long amount);
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsCalibrationElement();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationArrayData_Dispatch wrapper class

class CalibrationArrayData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationArrayData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationArrayData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationArrayData_Dispatch(const CalibrationArrayData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsReadOnly();
	BOOL SetBooleanImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetBooleanPhysValue(const VARIANT& value_Renamed);
	BOOL SetDoubleImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetDoublePhysValue(const VARIANT& value_Renamed);
	BOOL SetDoublePhysValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetFloatImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetFloatPhysValue(const VARIANT& value_Renamed);
	BOOL SetFloatPhysValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetImplValue(const VARIANT& value_Renamed);
	BOOL SetIntegerImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetIntegerPhysValue(const VARIANT& value_Renamed);
	BOOL SetLongImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetLongPhysValue(const VARIANT& value_Renamed);
	BOOL SetSize(long size);
	VARIANT GetBooleanImplValuesRange(long startIndex, long endIndex);
	VARIANT GetBooleanPhysValue();
	VARIANT GetDoubleImplValuesRange(long startIndex, long endIndex);
	VARIANT GetDoublePhysValue();
	VARIANT GetDoublePhysValuesRange(long startIndex, long endIndex);
	VARIANT GetFloatImplValuesRange(long startIndex, long endIndex);
	VARIANT GetFloatPhysValue();
	VARIANT GetFloatPhysValuesRange(long startIndex, long endIndex);
	CString GetImplType();
	VARIANT GetImplValue();
	VARIANT GetIntegerImplValuesRange(long startIndex, long endIndex);
	VARIANT GetIntegerPhysValue();
	VARIANT GetLongImplValuesRange(long startIndex, long endIndex);
	VARIANT GetLongPhysValue();
	long GetMaxSize();
	CString GetPhysType();
	long GetSize();
	BOOL IsArray();
	BOOL IsWriteProtected();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationCurveAxisData_Dispatch wrapper class

class CalibrationCurveAxisData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationCurveAxisData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationCurveAxisData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationCurveAxisData_Dispatch(const CalibrationCurveAxisData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsCurveAxis();
	BOOL IsOneDTable();
	LPDISPATCH GetDistribution();
	LPDISPATCH GetValue();
	BOOL SetDistribution(LPDISPATCH dispatch_dis);
	BOOL SetValue(LPDISPATCH dispatch_array);
	double GetCalibrationImplXAxisMinIncrement();
	double GetCalibrationImplXAxisWeakBoundMax();
	double GetCalibrationImplXAxisWeakBoundMin();
	double GetCalibrationXAxisMinIncrement();
	CString GetCalibrationXAxisSourceLabel();
	double GetCalibrationXAxisWeakBoundMax();
	double GetCalibrationXAxisWeakBoundMin();
	BOOL SetXAxisWeakBoundtoHardBound();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationCurveAxisOneDTableData_Dispatch wrapper class

class CalibrationCurveAxisOneDTableData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationCurveAxisOneDTableData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationCurveAxisOneDTableData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationCurveAxisOneDTableData_Dispatch(const CalibrationCurveAxisOneDTableData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetCalculatedDistribution();
	LPDISPATCH GetCurveAxis();
	LPDISPATCH GetValue();
	BOOL IsCurveAxisOneDTable();
	BOOL SetValue(LPDISPATCH dispatch_array);
	double GetCalibrationImplXAxisMinIncrement();
	double GetCalibrationImplXAxisWeakBoundMax();
	double GetCalibrationImplXAxisWeakBoundMin();
	double GetCalibrationXAxisMinIncrement();
	CString GetCalibrationXAxisSourceLabel();
	double GetCalibrationXAxisWeakBoundMax();
	double GetCalibrationXAxisWeakBoundMin();
	BOOL SetXAxisWeakBoundtoHardBound();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationCurveAxisTwoDTableData_Dispatch wrapper class

class CalibrationCurveAxisTwoDTableData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationCurveAxisTwoDTableData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationCurveAxisTwoDTableData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationCurveAxisTwoDTableData_Dispatch(const CalibrationCurveAxisTwoDTableData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetCalculatedXDistribution();
	LPDISPATCH GetCalculatedYDistribution();
	LPDISPATCH GetValue();
	LPDISPATCH GetXCurveAxis();
	LPDISPATCH GetYCurveAxis();
	BOOL IsCurveAxisTwoDTable();
	BOOL SetValue(LPDISPATCH dispatch_matrix);
	double GetCalibrationImplYAxisMinIncrement();
	double GetCalibrationImplYAxisWeakBoundMax();
	double GetCalibrationImplYAxisWeakBoundMin();
	double GetCalibrationYAxisMinIncrement();
	CString GetCalibrationYAxisSourceLabel();
	double GetCalibrationYAxisWeakBoundMax();
	double GetCalibrationYAxisWeakBoundMin();
	BOOL SetYAxisWeakBoundtoHardBound();
	double GetCalibrationImplXAxisMinIncrement();
	double GetCalibrationImplXAxisWeakBoundMax();
	double GetCalibrationImplXAxisWeakBoundMin();
	double GetCalibrationXAxisMinIncrement();
	CString GetCalibrationXAxisSourceLabel();
	double GetCalibrationXAxisWeakBoundMax();
	double GetCalibrationXAxisWeakBoundMin();
	BOOL SetXAxisWeakBoundtoHardBound();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationDataItem_Dispatch wrapper class

class CalibrationDataItem_Dispatch : public COleDispatchDriver
{
public:
	CalibrationDataItem_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationDataItem_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationDataItem_Dispatch(const CalibrationDataItem_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationDistributionData_Dispatch wrapper class

class CalibrationDistributionData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationDistributionData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationDistributionData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationDistributionData_Dispatch(const CalibrationDistributionData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsArray();
	BOOL IsDistribution();
	BOOL IsReadOnly();
	BOOL SetBooleanImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetBooleanPhysValue(const VARIANT& value_Renamed);
	BOOL SetDoubleImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetDoublePhysValue(const VARIANT& value_Renamed);
	BOOL SetDoublePhysValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetFloatImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetFloatPhysValue(const VARIANT& value_Renamed);
	BOOL SetFloatPhysValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetImplValue(const VARIANT& value_Renamed);
	BOOL SetIntegerImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetIntegerPhysValue(const VARIANT& value_Renamed);
	BOOL SetLongImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetLongPhysValue(const VARIANT& value_Renamed);
	BOOL SetSize(long size);
	VARIANT GetBooleanImplValuesRange(long startIndex, long endIndex);
	VARIANT GetBooleanPhysValue();
	VARIANT GetDoubleImplValuesRange(long startIndex, long endIndex);
	VARIANT GetDoublePhysValue();
	VARIANT GetDoublePhysValuesRange(long startIndex, long endIndex);
	VARIANT GetFloatImplValuesRange(long startIndex, long endIndex);
	VARIANT GetFloatPhysValue();
	VARIANT GetFloatPhysValuesRange(long startIndex, long endIndex);
	CString GetImplType();
	VARIANT GetImplValue();
	VARIANT GetIntegerImplValuesRange(long startIndex, long endIndex);
	VARIANT GetIntegerPhysValue();
	VARIANT GetLongImplValuesRange(long startIndex, long endIndex);
	VARIANT GetLongPhysValue();
	long GetMaxSize();
	CString GetPhysType();
	long GetSize();
	BOOL IsWriteProtected();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationElement_Dispatch wrapper class

class CalibrationElement_Dispatch : public COleDispatchDriver
{
public:
	CalibrationElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationElement_Dispatch(const CalibrationElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsCalibrationElement();
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsMeasureElement();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationMatrixData_Dispatch wrapper class

class CalibrationMatrixData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationMatrixData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationMatrixData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationMatrixData_Dispatch(const CalibrationMatrixData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetBooleanImplValuesRange(long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	VARIANT GetBooleanPhysValue();
	VARIANT GetDoubleImplValuesRange(long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	VARIANT GetDoublePhysValue();
	VARIANT GetDoublePhysValuesRange(long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	VARIANT GetFloatImplValuesRange(long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	VARIANT GetFloatPhysValue();
	VARIANT GetFloatPhysValuesRange(long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	CString GetImplType();
	VARIANT GetImplValue();
	VARIANT GetIntegerImplValuesRange(long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	VARIANT GetIntegerPhysValue();
	VARIANT GetLongImplValuesRange(long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	VARIANT GetLongPhysValue();
	long GetMaxXSize();
	long GetMaxYSize();
	CString GetPhysType();
	long GetXSize();
	long GetYSize();
	BOOL IsMatrix();
	BOOL IsWriteProtected();
	BOOL SetBooleanImplValuesRange(const VARIANT& value_Renamed, long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	BOOL SetBooleanPhysValue(const VARIANT& value_Renamed);
	BOOL SetDoubleImplValuesRange(const VARIANT& value_Renamed, long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	BOOL SetDoublePhysValue(const VARIANT& value_Renamed);
	BOOL SetDoublePhysValuesRange(const VARIANT& value_Renamed, long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	BOOL SetFloatImplValuesRange(const VARIANT& value_Renamed, long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	BOOL SetFloatPhysValue(const VARIANT& value_Renamed);
	BOOL SetFloatPhysValuesRange(const VARIANT& value_Renamed, long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	BOOL SetImplValue(const VARIANT& value_Renamed);
	BOOL SetIntegerImplValuesRange(const VARIANT& value_Renamed, long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	BOOL SetIntegerPhysValue(const VARIANT& value_Renamed);
	BOOL SetLongImplValuesRange(const VARIANT& value_Renamed, long startXIndex, long startYIndex, long endXIndex, long endYIndex);
	BOOL SetLongPhysValue(const VARIANT& value_Renamed);
	BOOL SetXSize(long xSize);
	BOOL SetYSize(long ySize);
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationOneDTableData_Dispatch wrapper class

class CalibrationOneDTableData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationOneDTableData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationOneDTableData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationOneDTableData_Dispatch(const CalibrationOneDTableData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetDistribution();
	LPDISPATCH GetValue();
	BOOL IsOneDTable();
	BOOL SetDistribution(LPDISPATCH dispatch_dis);
	BOOL SetValue(LPDISPATCH dispatch_array);
	double GetCalibrationImplXAxisMinIncrement();
	double GetCalibrationImplXAxisWeakBoundMax();
	double GetCalibrationImplXAxisWeakBoundMin();
	double GetCalibrationXAxisMinIncrement();
	CString GetCalibrationXAxisSourceLabel();
	double GetCalibrationXAxisWeakBoundMax();
	double GetCalibrationXAxisWeakBoundMin();
	BOOL SetXAxisWeakBoundtoHardBound();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationReadOnlyArray_Dispatch wrapper class

class CalibrationReadOnlyArray_Dispatch : public COleDispatchDriver
{
public:
	CalibrationReadOnlyArray_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationReadOnlyArray_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationReadOnlyArray_Dispatch(const CalibrationReadOnlyArray_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetBooleanImplValuesRange(long startIndex, long endIndex);
	VARIANT GetBooleanPhysValue();
	VARIANT GetDoubleImplValuesRange(long startIndex, long endIndex);
	VARIANT GetDoublePhysValue();
	VARIANT GetDoublePhysValuesRange(long startIndex, long endIndex);
	VARIANT GetFloatImplValuesRange(long startIndex, long endIndex);
	VARIANT GetFloatPhysValue();
	VARIANT GetFloatPhysValuesRange(long startIndex, long endIndex);
	CString GetImplType();
	VARIANT GetImplValue();
	VARIANT GetIntegerImplValuesRange(long startIndex, long endIndex);
	VARIANT GetIntegerPhysValue();
	VARIANT GetLongImplValuesRange(long startIndex, long endIndex);
	VARIANT GetLongPhysValue();
	long GetMaxSize();
	CString GetPhysType();
	long GetSize();
	BOOL IsArray();
	BOOL IsReadOnly();
	BOOL IsWriteProtected();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationReadOnlyDistributionData_Dispatch wrapper class

class CalibrationReadOnlyDistributionData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationReadOnlyDistributionData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationReadOnlyDistributionData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationReadOnlyDistributionData_Dispatch(const CalibrationReadOnlyDistributionData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsArray();
	BOOL IsDistribution();
	VARIANT GetBooleanImplValuesRange(long startIndex, long endIndex);
	VARIANT GetBooleanPhysValue();
	VARIANT GetDoubleImplValuesRange(long startIndex, long endIndex);
	VARIANT GetDoublePhysValue();
	VARIANT GetDoublePhysValuesRange(long startIndex, long endIndex);
	VARIANT GetFloatImplValuesRange(long startIndex, long endIndex);
	VARIANT GetFloatPhysValue();
	VARIANT GetFloatPhysValuesRange(long startIndex, long endIndex);
	CString GetImplType();
	VARIANT GetImplValue();
	VARIANT GetIntegerImplValuesRange(long startIndex, long endIndex);
	VARIANT GetIntegerPhysValue();
	VARIANT GetLongImplValuesRange(long startIndex, long endIndex);
	VARIANT GetLongPhysValue();
	long GetMaxSize();
	CString GetPhysType();
	long GetSize();
	BOOL IsReadOnly();
	BOOL IsWriteProtected();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationRescaleDistributionData_Dispatch wrapper class

class CalibrationRescaleDistributionData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationRescaleDistributionData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationRescaleDistributionData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationRescaleDistributionData_Dispatch(const CalibrationRescaleDistributionData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetDoublePhysScales();
	VARIANT GetFloatPhysScales();
	VARIANT GetIntegerPhysScales();
	VARIANT GetLongPhysScales();
	BOOL IsRescaleDistribution();
	BOOL SetDoublePhysScales(const VARIANT& value_Renamed);
	BOOL SetFloatPhysScales(const VARIANT& value_Renamed);
	BOOL SetIntegerPhysScales(const VARIANT& value_Renamed);
	BOOL SetLongPhysScales(const VARIANT& value_Renamed);
	BOOL IsArray();
	BOOL IsDistribution();
	BOOL IsReadOnly();
	BOOL SetBooleanImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetBooleanPhysValue(const VARIANT& value_Renamed);
	BOOL SetDoubleImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetDoublePhysValue(const VARIANT& value_Renamed);
	BOOL SetDoublePhysValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetFloatImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetFloatPhysValue(const VARIANT& value_Renamed);
	BOOL SetFloatPhysValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetImplValue(const VARIANT& value_Renamed);
	BOOL SetIntegerImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetIntegerPhysValue(const VARIANT& value_Renamed);
	BOOL SetLongImplValuesRange(const VARIANT& value_Renamed, long startIndex, long endIndex);
	BOOL SetLongPhysValue(const VARIANT& value_Renamed);
	BOOL SetSize(long size);
	VARIANT GetBooleanImplValuesRange(long startIndex, long endIndex);
	VARIANT GetBooleanPhysValue();
	VARIANT GetDoubleImplValuesRange(long startIndex, long endIndex);
	VARIANT GetDoublePhysValue();
	VARIANT GetDoublePhysValuesRange(long startIndex, long endIndex);
	VARIANT GetFloatImplValuesRange(long startIndex, long endIndex);
	VARIANT GetFloatPhysValue();
	VARIANT GetFloatPhysValuesRange(long startIndex, long endIndex);
	CString GetImplType();
	VARIANT GetImplValue();
	VARIANT GetIntegerImplValuesRange(long startIndex, long endIndex);
	VARIANT GetIntegerPhysValue();
	VARIANT GetLongImplValuesRange(long startIndex, long endIndex);
	VARIANT GetLongPhysValue();
	long GetMaxSize();
	CString GetPhysType();
	long GetSize();
	BOOL IsWriteProtected();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationRescaleOneDTableData_Dispatch wrapper class

class CalibrationRescaleOneDTableData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationRescaleOneDTableData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationRescaleOneDTableData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationRescaleOneDTableData_Dispatch(const CalibrationRescaleOneDTableData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetCalculatedDistribution();
	LPDISPATCH GetDistribution();
	BOOL IsOneDTable();
	BOOL IsRescaleOneDTable();
	BOOL SetDistribution(LPDISPATCH dispatch_dis);
	LPDISPATCH GetValue();
	BOOL SetValue(LPDISPATCH dispatch_array);
	double GetCalibrationImplXAxisMinIncrement();
	double GetCalibrationImplXAxisWeakBoundMax();
	double GetCalibrationImplXAxisWeakBoundMin();
	double GetCalibrationXAxisMinIncrement();
	CString GetCalibrationXAxisSourceLabel();
	double GetCalibrationXAxisWeakBoundMax();
	double GetCalibrationXAxisWeakBoundMin();
	BOOL SetXAxisWeakBoundtoHardBound();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationRescaleTwoDTableData_Dispatch wrapper class

class CalibrationRescaleTwoDTableData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationRescaleTwoDTableData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationRescaleTwoDTableData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationRescaleTwoDTableData_Dispatch(const CalibrationRescaleTwoDTableData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetCalculatedXDistribution();
	LPDISPATCH GetCalculatedYDistribution();
	LPDISPATCH GetXDistribution();
	LPDISPATCH GetYDistribution();
	BOOL IsRescaleTwoDTable();
	BOOL IsTwoDTable();
	BOOL SetXDistribution(LPDISPATCH dispatch_dis);
	BOOL SetYDistribution(LPDISPATCH dispatch_dis);
	LPDISPATCH GetValue();
	BOOL SetValue(LPDISPATCH dispatch_matrix);
	double GetCalibrationImplYAxisMinIncrement();
	double GetCalibrationImplYAxisWeakBoundMax();
	double GetCalibrationImplYAxisWeakBoundMin();
	double GetCalibrationYAxisMinIncrement();
	CString GetCalibrationYAxisSourceLabel();
	double GetCalibrationYAxisWeakBoundMax();
	double GetCalibrationYAxisWeakBoundMin();
	BOOL SetYAxisWeakBoundtoHardBound();
	double GetCalibrationImplXAxisMinIncrement();
	double GetCalibrationImplXAxisWeakBoundMax();
	double GetCalibrationImplXAxisWeakBoundMin();
	double GetCalibrationXAxisMinIncrement();
	CString GetCalibrationXAxisSourceLabel();
	double GetCalibrationXAxisWeakBoundMax();
	double GetCalibrationXAxisWeakBoundMin();
	BOOL SetXAxisWeakBoundtoHardBound();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsScalar();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationScalarData_Dispatch wrapper class

class CalibrationScalarData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationScalarData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationScalarData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationScalarData_Dispatch(const CalibrationScalarData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL GetBooleanImplValue();
	BOOL GetBooleanPhysValue();
	CString GetCharPhysValue();
	double GetDoubleImplValue();
	double GetDoublePhysValue();
	double GetFloatImplValue();
	double GetFloatPhysValue();
	CString GetImplType();
	double GetImplValue();
	long GetIntegerImplValue();
	long GetIntegerPhysValue();
	long GetLongImplValue();
	long GetLongPhysValue();
	CString GetPhysType();
	BOOL IsScalar();
	BOOL IsWriteProtected();
	BOOL SetBooleanImplValue(BOOL value_Renamed);
	BOOL SetBooleanPhysValue(BOOL value_Renamed);
	BOOL SetCharPhysValue(LPCTSTR value_Renamed);
	BOOL SetDoublePhysValue(double value_Renamed);
	BOOL SetFloatPhysValue(double value_Renamed);
	BOOL SetImplValue(double value_Renamed);
	BOOL SetIntegerImplValue(long value_Renamed);
	BOOL SetIntegerPhysValue(long value_Renamed);
	BOOL SetLongImplValue(long value_Renamed);
	BOOL SetLongPhysValue(long value_Renamed);
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// CalibrationTwoDTableData_Dispatch wrapper class

class CalibrationTwoDTableData_Dispatch : public COleDispatchDriver
{
public:
	CalibrationTwoDTableData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	CalibrationTwoDTableData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CalibrationTwoDTableData_Dispatch(const CalibrationTwoDTableData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetValue();
	LPDISPATCH GetXDistribution();
	LPDISPATCH GetYDistribution();
	BOOL IsTwoDTable();
	BOOL SetValue(LPDISPATCH dispatch_matrix);
	BOOL SetXDistribution(LPDISPATCH dispatch_dis);
	BOOL SetYDistribution(LPDISPATCH dispatch_dis);
	double GetCalibrationImplYAxisMinIncrement();
	double GetCalibrationImplYAxisWeakBoundMax();
	double GetCalibrationImplYAxisWeakBoundMin();
	double GetCalibrationYAxisMinIncrement();
	CString GetCalibrationYAxisSourceLabel();
	double GetCalibrationYAxisWeakBoundMax();
	double GetCalibrationYAxisWeakBoundMin();
	BOOL SetYAxisWeakBoundtoHardBound();
	double GetCalibrationImplXAxisMinIncrement();
	double GetCalibrationImplXAxisWeakBoundMax();
	double GetCalibrationImplXAxisWeakBoundMin();
	double GetCalibrationXAxisMinIncrement();
	CString GetCalibrationXAxisSourceLabel();
	double GetCalibrationXAxisWeakBoundMax();
	double GetCalibrationXAxisWeakBoundMin();
	BOOL SetXAxisWeakBoundtoHardBound();
	long GetAddress();
	CString GetAddressAsHexString();
	double GetCalibrationHardBoundMax();
	double GetCalibrationHardBoundMin();
	double GetCalibrationImplHardBoundMax();
	double GetCalibrationImplHardBoundMin();
	double GetCalibrationImplMinIncrement();
	double GetCalibrationImplWeakBoundMax();
	double GetCalibrationImplWeakBoundMin();
	double GetCalibrationMinIncrement();
	double GetCalibrationWeakBoundMax();
	double GetCalibrationWeakBoundMin();
	BOOL IsCalibrationDataItem();
	BOOL SetWeakBoundtoHardBound();
	BOOL IsArray();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Component_Dispatch wrapper class

class Component_Dispatch : public COleDispatchDriver
{
public:
	Component_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Component_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Component_Dispatch(const Component_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL AddTextSegment(LPCTSTR textSegment, const VARIANT& views);
	BOOL AssignViewToTextSegment(LPCTSTR View, LPCTSTR textSegment);
	BOOL DeAssignViewFromTextSegment(LPCTSTR View, LPCTSTR textSegment);
	LPDISPATCH Edit();
	LPDISPATCH EditInFront();
	CString GetConfiguration();
	CString GetDate();
	CString GetNoteForTextSegment(LPCTSTR textSegment);
	VARIANT GetTextSegments();
	CString GetVersion();
	VARIANT GetViewsForTextSegment(LPCTSTR textSegment);
	BOOL IsComponent();
	BOOL IsEdition();
	BOOL IsVersion();
	BOOL RemoveTextSegment(LPCTSTR textSegment);
	BOOL ResetVersion();
	BOOL SetConfiguration(LPCTSTR configuration);
	BOOL SetEdition();
	BOOL SetNoteForTextSegment(LPCTSTR noteText, LPCTSTR textSegment);
	BOOL SetVersion(LPCTSTR version);
	LPDISPATCH Copy(LPCTSTR name);
	LPDISPATCH CopyToFolder(LPCTSTR name, LPDISPATCH dispatch_folder);
	BOOL ExportToFile(LPCTSTR fileName, BOOL multipleFiles, BOOL reference);
	BOOL GetAccessRightApplication();
	BOOL GetAccessRightCodeGeneration();
	BOOL GetAccessRightExecute();
	BOOL GetAccessRightRead();
	BOOL GetAccessRightWrite();
	VARIANT GetAllReferecedDataBaseItems();
	CString GetComment();
	BOOL GetDisallowImport();
	CString GetName();
	CString GetNameWithPath();
	CString GetOID();
	LPDISPATCH GetParentFolder();
	CString GetType();
	BOOL IsAsap2CanDB();
	BOOL IsAsap2Catalog();
	BOOL IsAsap2Item();
	BOOL IsAsap2Project();
	BOOL IsAutosarFolder();
	BOOL IsAsap2ProjectFolder();
	BOOL IsAscetContainer();
	BOOL IsAscetFolder();
	BOOL IsAscetNetwork();
	BOOL IsAscetProject();
	BOOL IsAscetProtocol();
	BOOL IsClass();
	BOOL IsCodeComponent();
	BOOL IsConditionalTable();
	BOOL IsCTBlock();
	BOOL IsCTComponent();
	BOOL IsDataBaseItem();
	BOOL IsDataSet();
	BOOL IsDiscreteComponent();
	BOOL IsEnumeration();
	BOOL IsExperimentEnvironment();
	BOOL IsODXProject();
	BOOL IsFolder();
	BOOL IsFunctionalComponent();
	BOOL IsHardwareConfiguration();
	BOOL IsIcon();
	BOOL IsIncaFolder();
	BOOL IsModule();
	BOOL IsProject();
	BOOL IsSignal();
	BOOL IsStateMachine();
	BOOL Move(LPDISPATCH dispatch_folder);
	BOOL SetAccessRightApplication(BOOL aBool);
	BOOL SetAccessRightCodeGeneration(BOOL aBool);
	BOOL SetAccessRightExecute(BOOL aBool);
	BOOL SetAccessRightRead(BOOL aBool);
	BOOL SetAccessRightWrite(BOOL aBool);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetDisallowImport(BOOL aBool);
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// DataBase_Dispatch wrapper class

class DataBase_Dispatch : public COleDispatchDriver
{
public:
	DataBase_Dispatch() {}		// Calls COleDispatchDriver default constructor
	DataBase_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	DataBase_Dispatch(const DataBase_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH AddFolder(LPCTSTR folderName);
	VARIANT BrowseItem(LPCTSTR itemName);
	VARIANT BrowseItemInFolder(LPCTSTR itemName, LPCTSTR folderName);
	BOOL Close();
	void DisableAutoCommit();
	void EnableAutoCommit();
	BOOL ExportToFile(const VARIANT& items, LPCTSTR aFilePath, BOOL multipleFiles, BOOL withReferences);
	VARIANT GetAllFolders();
	LPDISPATCH GetFolder(LPCTSTR folderName);
	LPDISPATCH GetItem(LPCTSTR itemName);
	LPDISPATCH GetItemForOID(LPCTSTR oid);
	LPDISPATCH GetItemInFolder(LPCTSTR itemName, LPCTSTR folderName);
	CString GetName();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL ImportFromFile(LPCTSTR aFileName, BOOL overwriteFlag, BOOL discardImplFlag);
	VARIANT ImportItemsFromFile(LPCTSTR aFileName, BOOL overwriteFlag, BOOL discardImplFlag);
	BOOL IsAscetDataBase();
	BOOL IsDataBase();
	BOOL IsIncaDataBase();
	BOOL IsToolErrorAvailable();
	BOOL Remove(LPDISPATCH dispatch_aFolderItem, BOOL recursive);
	BOOL Save();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// DataBaseItem_Dispatch wrapper class

class DataBaseItem_Dispatch : public COleDispatchDriver
{
public:
	DataBaseItem_Dispatch() {}		// Calls COleDispatchDriver default constructor
	DataBaseItem_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	DataBaseItem_Dispatch(const DataBaseItem_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH Copy(LPCTSTR name);
	LPDISPATCH CopyToFolder(LPCTSTR name, LPDISPATCH dispatch_folder);
	BOOL ExportToFile(LPCTSTR fileName, BOOL multipleFiles, BOOL reference);
	BOOL GetAccessRightApplication();
	BOOL GetAccessRightCodeGeneration();
	BOOL GetAccessRightExecute();
	BOOL GetAccessRightRead();
	BOOL GetAccessRightWrite();
	VARIANT GetAllReferecedDataBaseItems();
	CString GetComment();
	BOOL GetDisallowImport();
	CString GetName();
	CString GetNameWithPath();
	CString GetOID();
	LPDISPATCH GetParentFolder();
	CString GetType();
	BOOL IsAsap2CanDB();
	BOOL IsAsap2Catalog();
	BOOL IsAsap2Item();
	BOOL IsAsap2Project();
	BOOL IsAutosarFolder();
	BOOL IsAsap2ProjectFolder();
	BOOL IsAscetContainer();
	BOOL IsAscetFolder();
	BOOL IsAscetNetwork();
	BOOL IsAscetProject();
	BOOL IsAscetProtocol();
	BOOL IsClass();
	BOOL IsCodeComponent();
	BOOL IsComponent();
	BOOL IsConditionalTable();
	BOOL IsCTBlock();
	BOOL IsCTComponent();
	BOOL IsDataBaseItem();
	BOOL IsDataSet();
	BOOL IsDiscreteComponent();
	BOOL IsEnumeration();
	BOOL IsExperimentEnvironment();
	BOOL IsODXProject();
	BOOL IsFolder();
	BOOL IsFunctionalComponent();
	BOOL IsHardwareConfiguration();
	BOOL IsIcon();
	BOOL IsIncaFolder();
	BOOL IsModule();
	BOOL IsProject();
	BOOL IsSignal();
	BOOL IsStateMachine();
	BOOL Move(LPDISPATCH dispatch_folder);
	BOOL SetAccessRightApplication(BOOL aBool);
	BOOL SetAccessRightCodeGeneration(BOOL aBool);
	BOOL SetAccessRightExecute(BOOL aBool);
	BOOL SetAccessRightRead(BOOL aBool);
	BOOL SetAccessRightWrite(BOOL aBool);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetDisallowImport(BOOL aBool);
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// DistributionCalibrationElement_Dispatch wrapper class

class DistributionCalibrationElement_Dispatch : public COleDispatchDriver
{
public:
	DistributionCalibrationElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	DistributionCalibrationElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	DistributionCalibrationElement_Dispatch(const DistributionCalibrationElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsDistribution();
	BOOL IsCalibrationElement();
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsMeasureElement();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// EtasLicenseFeature_Dispatch wrapper class

class EtasLicenseFeature_Dispatch : public COleDispatchDriver
{
public:
	EtasLicenseFeature_Dispatch() {}		// Calls COleDispatchDriver default constructor
	EtasLicenseFeature_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	EtasLicenseFeature_Dispatch(const EtasLicenseFeature_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL CheckIn();
	BOOL CheckOut();
	BOOL IsCheckOut();
	BOOL IsSystemDefined();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// EtasLicenseManager_Dispatch wrapper class

class EtasLicenseManager_Dispatch : public COleDispatchDriver
{
public:
	EtasLicenseManager_Dispatch() {}		// Calls COleDispatchDriver default constructor
	EtasLicenseManager_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	EtasLicenseManager_Dispatch(const EtasLicenseManager_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH CreateUserDefinedFeature(LPCTSTR name, LPCTSTR featureName, long majorVersion, long minorVersion);
	VARIANT GetAllAvailableFeatures();
	LPDISPATCH GetFeatureNamed(LPCTSTR name);
	BOOL Heartbeat();
	BOOL IsActive();
	BOOL WriteToLogFile(LPCTSTR text);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Experiment_Dispatch wrapper class

class Experiment_Dispatch : public COleDispatchDriver
{
public:
	Experiment_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Experiment_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Experiment_Dispatch(const Experiment_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetEtas_binary();
	CString GetDiadem_atf();
	CString GetEtas_ascii();
	CString GetEtas_groupascii();
	CString GetMatlab_mfile();
	CString GetFamos_record();
	CString GetEtas_mdf();
	CString GetEtas_mdf4();
	VARIANT BrowseCalibrationElement(LPCTSTR name);
	VARIANT BrowseCalibrationElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseCaseInsensitiveCalibrationElement(LPCTSTR name);
	VARIANT BrowseCaseInsensitiveCalibrationElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseCaseInsensitiveExperimentElement(LPCTSTR name);
	VARIANT BrowseCaseInsensitiveExperimentElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseCaseInsensitiveMeasureElement(LPCTSTR name);
	VARIANT BrowseCaseInsensitiveMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseExperimentElement(LPCTSTR name);
	VARIANT BrowseExperimentElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseMeasureElement(LPCTSTR name);
	VARIANT BrowseMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	long CheckTypeForElementNameInDevice(LPCTSTR aLabelName, LPDISPATCH dispatch_aDevice);
	VARIANT GetAllCalibrationElements();
	VARIANT GetAllCalibrationElementsInDevice(LPDISPATCH dispatch_device);
	VARIANT GetAllDevices();
	VARIANT GetAllExperimentElements();
	VARIANT GetAllExperimentElementsInDevice(LPDISPATCH dispatch_device);
	VARIANT GetAllMeasureElements();
	VARIANT GetAllMeasureElementsInDevice(LPDISPATCH dispatch_device);
	LPDISPATCH GetCalibrationElement(LPCTSTR name);
	LPDISPATCH GetCalibrationElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetCalibrationValue(LPCTSTR name);
	LPDISPATCH GetCalibrationValueInDevice(LPCTSTR name, LPDISPATCH dispatch_dev);
	CString GetDefaultRasterForMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetDevice(LPCTSTR name);
	LPDISPATCH GetExperimentElement(LPCTSTR name);
	LPDISPATCH GetExperimentElementForCaseInsensitiveLabel(LPCTSTR name);
	LPDISPATCH GetExperimentElementForCaseInsensitiveLabelInDevice(LPCTSTR name, LPDISPATCH dispatch_dev);
	LPDISPATCH GetExperimentElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetMeasureElement(LPCTSTR name);
	LPDISPATCH GetMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetMeasureValue(LPCTSTR name);
	LPDISPATCH GetMeasureValueInDevice(LPCTSTR name, LPDISPATCH dispatch_dev);
	LPDISPATCH GetMeasureValueWithAcquisitionRate(LPCTSTR name, LPCTSTR acquisitionRate);
	LPDISPATCH GetMeasureValueWithAcquisitionRateInDevice(LPCTSTR name, LPCTSTR acquisitionRate, LPDISPATCH dispatch_dev);
	VARIANT GetMeasurementsWithRCI2(const VARIANT& devNames, const VARIANT& rasterNames, const VARIANT& messNames);
	BOOL GetRecordingState();
	BOOL IsExperiment();
	BOOL IsIncaOnlineExperiment();
	BOOL IsOnlineExperiment();
	BOOL PauseRecording();
	BOOL StartRecording();
	BOOL StopAndDiscardRecording();
	BOOL StopRecording(LPCTSTR fileName, LPCTSTR fileFormat);
	BOOL ConvertMdfFile(LPCTSTR fileName, LPCTSTR fileFormat);
	VARIANT GetHWDevicesFlat(LPDISPATCH dispatch_aHWConfig);
	VARIANT GetDependentCommonAxisConsumers(LPCTSTR elementName);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// ExperimentDataItem_Dispatch wrapper class

class ExperimentDataItem_Dispatch : public COleDispatchDriver
{
public:
	ExperimentDataItem_Dispatch() {}		// Calls COleDispatchDriver default constructor
	ExperimentDataItem_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ExperimentDataItem_Dispatch(const ExperimentDataItem_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsArray();
	BOOL IsCalibrationDataItem();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsMeasureDataItem();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// ExperimentDevice_Dispatch wrapper class

class ExperimentDevice_Dispatch : public COleDispatchDriver
{
public:
	ExperimentDevice_Dispatch() {}		// Calls COleDispatchDriver default constructor
	ExperimentDevice_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ExperimentDevice_Dispatch(const ExperimentDevice_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT ActivateRci2ForAcquisitionRate(LPCTSTR name);
	BOOL CanAssignProject();
	BOOL DeActivateRci2ForAcquisitionRate(LPCTSTR name);
	double GetA2lScalingFactorOfSignalGroup(LPCTSTR signalGroupName);
	long GetA2lScalingUnitOfSignalGroup(LPCTSTR signalGroupName);
	VARIANT GetAllAcquisitionRates();
	LPDISPATCH GetASAP2Module();
	CString GetDeviceType();
	CString GetName();
	CString GetProjectDataBasePath();
	double GetSampleRateFor(LPCTSTR aName);
	BOOL IsAcquisitionRateAvailable(LPCTSTR rate);
	BOOL SetAsyncBuffersToInvalid();
	BOOL IsActive();
	BOOL IsCANMonitoringDevice();
	BOOL IsExperimentDevice();
	BOOL IsKwp2000Device();
	BOOL IsOutputDevice();
	BOOL IsWorkbaseDevice();
	BOOL IsEtherCATSupported();
	BOOL IsFULISupported();
	BOOL ResetTarget();
	BOOL StartTargetOS();
	BOOL StopTargetOS();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// ExperimentElement_Dispatch wrapper class

class ExperimentElement_Dispatch : public COleDispatchDriver
{
public:
	ExperimentElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	ExperimentElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ExperimentElement_Dispatch(const ExperimentElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsCalibrationElement();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsMeasureElement();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// ExperimentView_Dispatch wrapper class

class ExperimentView_Dispatch : public COleDispatchDriver
{
public:
	ExperimentView_Dispatch() {}		// Calls COleDispatchDriver default constructor
	ExperimentView_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ExperimentView_Dispatch(const ExperimentView_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL CloseAllAPICalibrationViews();
	BOOL CloseAllAPIMeasureViews();
	BOOL CloseAllViews();
	LPDISPATCH GetExperiment();
	BOOL IsExperimentChanged();
	BOOL IsExperimentView();
	BOOL IsGroupedDevices();
	BOOL IsIncaExperimentView();
	BOOL OpenViewForExperimentDataItem(LPDISPATCH dispatch_edi);
	BOOL OpenViewForExperimentElement(LPDISPATCH dispatch_el);
	BOOL OpenViewForMeasureDataItemWithMaxLabels(LPDISPATCH dispatch_mdi, long amount);
	BOOL OpenViewForMeasureElementWithMaxLabels(LPDISPATCH dispatch_el, long amount);
	BOOL OpenViewsForExperimentDataItems(const VARIANT& edi);
	BOOL OpenViewsForExperimentElements(const VARIANT& el);
	BOOL OpenViewsForMeasureDataItemsWithMaxLabels(const VARIANT& mdi, long amount);
	BOOL OpenViewsForMeasureElementsWithMaxLabels(const VARIANT& el, long amount);
	BOOL Quit();
	BOOL SaveExperiment();
	BOOL SetGroupedDevices(long onOff);
	BOOL UpdateViewForExperimentDataItem(LPDISPATCH dispatch_edi);
	BOOL UpdateViewForExperimentElement(LPDISPATCH dispatch_el);
	BOOL UpdateViewsForExperimentDataItems(const VARIANT& edi);
	BOOL UpdateViewsForExperimentElements(const VARIANT& el);
	BOOL WriteCalibrationElementsToDCMFile(const VARIANT& edi, LPCTSTR fileName);
	BOOL BringToFront();
	BOOL Close();
	BOOL IsView();
	BOOL IsDialog();
	BOOL IsUserInterface();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Folder_Dispatch wrapper class

class Folder_Dispatch : public COleDispatchDriver
{
public:
	Folder_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Folder_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Folder_Dispatch(const Folder_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH AddSubFolder(LPCTSTR folderName);
	VARIANT BrowseComponent(LPCTSTR name);
	VARIANT BrowseDataBaseItem(LPCTSTR name);
	VARIANT BrowseSubFolder(LPCTSTR name);
	VARIANT GetAllComponents();
	VARIANT GetAllDataBaseItems();
	VARIANT GetAllSubFolders();
	LPDISPATCH GetComponent(LPCTSTR componentName);
	LPDISPATCH GetDataBaseItem(LPCTSTR itemName);
	LPDISPATCH GetSubFolder(LPCTSTR folderName);
	BOOL IsFolder();
	BOOL RemoveComponent(LPDISPATCH dispatch_comp);
	BOOL RemoveFolder(LPDISPATCH dispatch_folder, BOOL recursive);
	LPDISPATCH Copy(LPCTSTR name);
	LPDISPATCH CopyToFolder(LPCTSTR name, LPDISPATCH dispatch_folder);
	BOOL ExportToFile(LPCTSTR fileName, BOOL multipleFiles, BOOL reference);
	BOOL GetAccessRightApplication();
	BOOL GetAccessRightCodeGeneration();
	BOOL GetAccessRightExecute();
	BOOL GetAccessRightRead();
	BOOL GetAccessRightWrite();
	VARIANT GetAllReferecedDataBaseItems();
	CString GetComment();
	BOOL GetDisallowImport();
	CString GetName();
	CString GetNameWithPath();
	CString GetOID();
	LPDISPATCH GetParentFolder();
	CString GetType();
	BOOL IsAsap2CanDB();
	BOOL IsAsap2Catalog();
	BOOL IsAsap2Item();
	BOOL IsAsap2Project();
	BOOL IsAutosarFolder();
	BOOL IsAsap2ProjectFolder();
	BOOL IsAscetContainer();
	BOOL IsAscetFolder();
	BOOL IsAscetNetwork();
	BOOL IsAscetProject();
	BOOL IsAscetProtocol();
	BOOL IsClass();
	BOOL IsCodeComponent();
	BOOL IsComponent();
	BOOL IsConditionalTable();
	BOOL IsCTBlock();
	BOOL IsCTComponent();
	BOOL IsDataBaseItem();
	BOOL IsDataSet();
	BOOL IsDiscreteComponent();
	BOOL IsEnumeration();
	BOOL IsExperimentEnvironment();
	BOOL IsODXProject();
	BOOL IsFunctionalComponent();
	BOOL IsHardwareConfiguration();
	BOOL IsIcon();
	BOOL IsIncaFolder();
	BOOL IsModule();
	BOOL IsProject();
	BOOL IsSignal();
	BOOL IsStateMachine();
	BOOL Move(LPDISPATCH dispatch_folder);
	BOOL SetAccessRightApplication(BOOL aBool);
	BOOL SetAccessRightCodeGeneration(BOOL aBool);
	BOOL SetAccessRightExecute(BOOL aBool);
	BOOL SetAccessRightRead(BOOL aBool);
	BOOL SetAccessRightWrite(BOOL aBool);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetDisallowImport(BOOL aBool);
	BOOL SetName(LPCTSTR aName);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDeviceDescription();
	BOOL IsFormula();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// Formula_Dispatch wrapper class

class Formula_Dispatch : public COleDispatchDriver
{
public:
	Formula_Dispatch() {}		// Calls COleDispatchDriver default constructor
	Formula_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Formula_Dispatch(const Formula_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT AvailableFormulaTypes();
	CString GetComment();
	CString GetContents();
	VARIANT GetFormulaParameters();
	CString GetFormulaType();
	CString GetName();
	CString GetUnit();
	BOOL Is5Parameters();
	BOOL IsFormula();
	BOOL IsIdentity();
	BOOL IsLinear();
	BOOL IsMoebius();
	BOOL Set5ParameterParameters(double p1, double p2, double p3, double p4, double p56);
	BOOL SetComment(LPCTSTR comment);
	BOOL SetLinearParameters(double c0, double c1);
	BOOL SetMoebiusParameters(double c0, double c1, double d0, double d1);
	BOOL SetName(LPCTSTR name);
	BOOL SetUnit(LPCTSTR unit);
	LPDISPATCH GetDataBase();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractBlockDiagramElement();
	BOOL IsAbstractMethod();
	BOOL IsAscetDiagram();
	BOOL IsAscetProjectFile();
	BOOL IsCodeGenerator();
	BOOL IsConfiguration();
	BOOL IsConfigurationItem();
	BOOL IsDataBaseItem();
	BOOL IsDeviceDescription();
	BOOL IsHWDevice();
	BOOL IsHWSubDevice();
	BOOL IsHWSystem();
	BOOL IsImplInfo();
	BOOL IsModelElement();
	BOOL IsSubDeviceDescription();
	BOOL IsSystemDescription();
	BOOL IsTarget();
	BOOL IsTask();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// MatrixCalibrationElement_Dispatch wrapper class

class MatrixCalibrationElement_Dispatch : public COleDispatchDriver
{
public:
	MatrixCalibrationElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	MatrixCalibrationElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	MatrixCalibrationElement_Dispatch(const MatrixCalibrationElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsMatrix();
	BOOL IsCalibrationElement();
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMeasureElement();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// MatrixMeasureElement_Dispatch wrapper class

class MatrixMeasureElement_Dispatch : public COleDispatchDriver
{
public:
	MatrixMeasureElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	MatrixMeasureElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	MatrixMeasureElement_Dispatch(const MatrixMeasureElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsMatrix();
	LPDISPATCH GetValueForAcquisitionRate(LPCTSTR rate);
	BOOL IsMeasureElement();
	BOOL OpenViewWithMaxLabels(long amount);
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsCalibrationElement();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// MeasureArrayData_Dispatch wrapper class

class MeasureArrayData_Dispatch : public COleDispatchDriver
{
public:
	MeasureArrayData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	MeasureArrayData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	MeasureArrayData_Dispatch(const MeasureArrayData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetBooleanPhysValue();
	VARIANT GetDoublePhysValue();
	VARIANT GetFloatPhysValue();
	VARIANT GetImplValue();
	VARIANT GetIntegerPhysValue();
	VARIANT GetLongPhysValue();
	long GetMaxSize();
	long GetSize();
	BOOL IsArray();
	VARIANT ActivateRci2();
	BOOL DeActivateRci2();
	CString GetCurrentAcquisitionRate();
	CString GetImplType();
	double GetMeasureRangeMax();
	double GetMeasureRangeMin();
	CString GetPhysType();
	BOOL IsMeasureDataItem();
	BOOL SetCurrentAcquisitionRate(LPCTSTR rate);
	BOOL IsCalibrationDataItem();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// MeasureDataItem_Dispatch wrapper class

class MeasureDataItem_Dispatch : public COleDispatchDriver
{
public:
	MeasureDataItem_Dispatch() {}		// Calls COleDispatchDriver default constructor
	MeasureDataItem_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	MeasureDataItem_Dispatch(const MeasureDataItem_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT ActivateRci2();
	BOOL DeActivateRci2();
	CString GetCurrentAcquisitionRate();
	CString GetImplType();
	double GetMeasureRangeMax();
	double GetMeasureRangeMin();
	CString GetPhysType();
	BOOL IsMeasureDataItem();
	BOOL SetCurrentAcquisitionRate(LPCTSTR rate);
	BOOL IsArray();
	BOOL IsCalibrationDataItem();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// MeasureElement_Dispatch wrapper class

class MeasureElement_Dispatch : public COleDispatchDriver
{
public:
	MeasureElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	MeasureElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	MeasureElement_Dispatch(const MeasureElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetValueForAcquisitionRate(LPCTSTR rate);
	BOOL IsMeasureElement();
	BOOL OpenViewWithMaxLabels(long amount);
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsCalibrationElement();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// MeasureMatrixData_Dispatch wrapper class

class MeasureMatrixData_Dispatch : public COleDispatchDriver
{
public:
	MeasureMatrixData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	MeasureMatrixData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	MeasureMatrixData_Dispatch(const MeasureMatrixData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetBooleanPhysValue();
	VARIANT GetDoublePhysValue();
	VARIANT GetFloatPhysValue();
	VARIANT GetImplValue();
	VARIANT GetIntegerPhysValue();
	VARIANT GetLongPhysValue();
	long GetMaxXSize();
	long GetMaxYSize();
	long GetXSize();
	long GetYSize();
	BOOL IsMatrix();
	VARIANT ActivateRci2();
	BOOL DeActivateRci2();
	CString GetCurrentAcquisitionRate();
	CString GetImplType();
	double GetMeasureRangeMax();
	double GetMeasureRangeMin();
	CString GetPhysType();
	BOOL IsMeasureDataItem();
	BOOL SetCurrentAcquisitionRate(LPCTSTR rate);
	BOOL IsArray();
	BOOL IsCalibrationDataItem();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// MeasureScalarData_Dispatch wrapper class

class MeasureScalarData_Dispatch : public COleDispatchDriver
{
public:
	MeasureScalarData_Dispatch() {}		// Calls COleDispatchDriver default constructor
	MeasureScalarData_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	MeasureScalarData_Dispatch(const MeasureScalarData_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	VARIANT GetRCI2SignalHandle();
	VARIANT GetRCI2GrpHandle();
	BOOL GetBooleanPhysValue();
	CString GetCharPhysValue();
	double GetDoublePhysValue();
	double GetFloatPhysValue();
	double GetImplValue();
	long GetIntegerPhysValue();
	long GetLongPhysValue();
	BOOL IsScalar();
	VARIANT ActivateRci2();
	BOOL DeActivateRci2();
	CString GetCurrentAcquisitionRate();
	CString GetImplType();
	double GetMeasureRangeMax();
	double GetMeasureRangeMin();
	CString GetPhysType();
	BOOL IsMeasureDataItem();
	BOOL SetCurrentAcquisitionRate(LPCTSTR rate);
	BOOL IsArray();
	BOOL IsCalibrationDataItem();
	BOOL IsCurveAxis();
	BOOL IsCurveAxisOneDTable();
	BOOL IsCurveAxisTwoDTable();
	BOOL IsDistribution();
	BOOL IsExperimentDataItem();
	BOOL IsMatrix();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsOneDTable();
	BOOL IsReadOnly();
	BOOL IsRescaleDistribution();
	BOOL IsRescaleOneDTable();
	BOOL IsRescaleTwoDTable();
	BOOL IsTwoDTable();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	CString Label();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// MessageBox_Dispatch wrapper class

class MessageBox_Dispatch : public COleDispatchDriver
{
public:
	MessageBox_Dispatch() {}		// Calls COleDispatchDriver default constructor
	MessageBox_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	MessageBox_Dispatch(const MessageBox_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL Close();
	BOOL IsDialog();
	BOOL IsUserInterface();
	BOOL IsView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// OneDTableCalibrationElement_Dispatch wrapper class

class OneDTableCalibrationElement_Dispatch : public COleDispatchDriver
{
public:
	OneDTableCalibrationElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	OneDTableCalibrationElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	OneDTableCalibrationElement_Dispatch(const OneDTableCalibrationElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsOneDTable();
	BOOL IsCalibrationElement();
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsMeasureElement();
	BOOL IsScalar();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// OnlineExperiment_Dispatch wrapper class

class OnlineExperiment_Dispatch : public COleDispatchDriver
{
public:
	OnlineExperiment_Dispatch() {}		// Calls COleDispatchDriver default constructor
	OnlineExperiment_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	OnlineExperiment_Dispatch(const OnlineExperiment_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetEtas_binary();
	CString GetDiadem_atf();
	CString GetEtas_ascii();
	CString GetEtas_groupascii();
	CString GetMatlab_mfile();
	CString GetFamos_record();
	CString GetEtas_mdf();
	CString GetEtas_mdf4();
	VARIANT GetAllExperimentSolverBlockLabels();
	VARIANT GetAllSolverIntegratorLabels();
	CString GetIntegratorNameForSolver(LPCTSTR aLabel);
	double GetIntegratorParameterForSolver(LPCTSTR parameterLabel, LPCTSTR aLabel);
	VARIANT GetIntegratorParameterLabels();
	BOOL IsExperimentLocked();
	BOOL IsMeasurementRunning();
	BOOL IsOnlineExperiment();
	BOOL LockExperiment();
	BOOL SetIntegratorAdamsMoulton2ForSolver(LPCTSTR aLabel, double dt, double h);
	BOOL SetIntegratorEulerForSolver(LPCTSTR aLabel, double dt, double h);
	BOOL SetIntegratorHeunForSolver(LPCTSTR aLabel, double dt, double h);
	BOOL SetIntegratorMulstep2ForSolver(LPCTSTR aLabel, double dt, double h);
	BOOL SetIntegratorParameterForSolver(LPCTSTR parameterLabel, LPCTSTR aLabel, double value_Renamed);
	BOOL SetIntegratorRungeKutta4ForSolver(LPCTSTR aLabel, double dt, double h);
	BOOL StartMeasurement();
	BOOL StopMeasurement();
	BOOL StopMeasurementAndSaveToFile(LPCTSTR fileName, LPCTSTR fileFormat);
	BOOL UnlockExperiment();
	VARIANT BrowseCalibrationElement(LPCTSTR name);
	VARIANT BrowseCalibrationElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseCaseInsensitiveCalibrationElement(LPCTSTR name);
	VARIANT BrowseCaseInsensitiveCalibrationElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseCaseInsensitiveExperimentElement(LPCTSTR name);
	VARIANT BrowseCaseInsensitiveExperimentElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseCaseInsensitiveMeasureElement(LPCTSTR name);
	VARIANT BrowseCaseInsensitiveMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseExperimentElement(LPCTSTR name);
	VARIANT BrowseExperimentElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	VARIANT BrowseMeasureElement(LPCTSTR name);
	VARIANT BrowseMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	long CheckTypeForElementNameInDevice(LPCTSTR aLabelName, LPDISPATCH dispatch_aDevice);
	VARIANT GetAllCalibrationElements();
	VARIANT GetAllCalibrationElementsInDevice(LPDISPATCH dispatch_device);
	VARIANT GetAllDevices();
	VARIANT GetAllExperimentElements();
	VARIANT GetAllExperimentElementsInDevice(LPDISPATCH dispatch_device);
	VARIANT GetAllMeasureElements();
	VARIANT GetAllMeasureElementsInDevice(LPDISPATCH dispatch_device);
	LPDISPATCH GetCalibrationElement(LPCTSTR name);
	LPDISPATCH GetCalibrationElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetCalibrationValue(LPCTSTR name);
	LPDISPATCH GetCalibrationValueInDevice(LPCTSTR name, LPDISPATCH dispatch_dev);
	CString GetDefaultRasterForMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetDevice(LPCTSTR name);
	LPDISPATCH GetExperimentElement(LPCTSTR name);
	LPDISPATCH GetExperimentElementForCaseInsensitiveLabel(LPCTSTR name);
	LPDISPATCH GetExperimentElementForCaseInsensitiveLabelInDevice(LPCTSTR name, LPDISPATCH dispatch_dev);
	LPDISPATCH GetExperimentElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetMeasureElement(LPCTSTR name);
	LPDISPATCH GetMeasureElementInDevice(LPCTSTR name, LPDISPATCH dispatch_device);
	LPDISPATCH GetMeasureValue(LPCTSTR name);
	LPDISPATCH GetMeasureValueInDevice(LPCTSTR name, LPDISPATCH dispatch_dev);
	LPDISPATCH GetMeasureValueWithAcquisitionRate(LPCTSTR name, LPCTSTR acquisitionRate);
	LPDISPATCH GetMeasureValueWithAcquisitionRateInDevice(LPCTSTR name, LPCTSTR acquisitionRate, LPDISPATCH dispatch_dev);
	VARIANT GetMeasurementsWithRCI2(const VARIANT& devNames, const VARIANT& rasterNames, const VARIANT& messNames);
	BOOL GetRecordingState();
	BOOL IsExperiment();
	BOOL IsIncaOnlineExperiment();
	BOOL PauseRecording();
	BOOL StartRecording();
	BOOL StopAndDiscardRecording();
	BOOL StopRecording(LPCTSTR fileName, LPCTSTR fileFormat);
	BOOL ConvertMdfFile(LPCTSTR fileName, LPCTSTR fileFormat);
	VARIANT GetHWDevicesFlat(LPDISPATCH dispatch_aHWConfig);
	VARIANT GetDependentCommonAxisConsumers(LPCTSTR elementName);
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// ScalarCalibrationElement_Dispatch wrapper class

class ScalarCalibrationElement_Dispatch : public COleDispatchDriver
{
public:
	ScalarCalibrationElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	ScalarCalibrationElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ScalarCalibrationElement_Dispatch(const ScalarCalibrationElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsScalar();
	BOOL IsCalibrationElement();
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsMeasureElement();
	BOOL IsOneDTable();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// ScalarMeasureElement_Dispatch wrapper class

class ScalarMeasureElement_Dispatch : public COleDispatchDriver
{
public:
	ScalarMeasureElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	ScalarMeasureElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ScalarMeasureElement_Dispatch(const ScalarMeasureElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsScalar();
	LPDISPATCH GetValueForAcquisitionRate(LPCTSTR rate);
	BOOL IsMeasureElement();
	BOOL OpenViewWithMaxLabels(long amount);
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsCalibrationElement();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsOneDTable();
	BOOL IsTwoDTable();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// TwoDTableCalibrationElement_Dispatch wrapper class

class TwoDTableCalibrationElement_Dispatch : public COleDispatchDriver
{
public:
	TwoDTableCalibrationElement_Dispatch() {}		// Calls COleDispatchDriver default constructor
	TwoDTableCalibrationElement_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	TwoDTableCalibrationElement_Dispatch(const TwoDTableCalibrationElement_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL IsTwoDTable();
	BOOL IsCalibrationElement();
	LPDISPATCH GetAsap2Label();
	LPDISPATCH GetDevice();
	CString GetName();
	LPDISPATCH GetValue();
	BOOL IsArray();
	BOOL IsDistribution();
	BOOL IsExperimentElement();
	BOOL IsMatrix();
	BOOL IsMeasureElement();
	BOOL IsOneDTable();
	BOOL IsScalar();
	BOOL IsCube3();
	BOOL IsCube4();
	BOOL IsThreeDTable();
	BOOL IsFourDTable();
	BOOL OpenView();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsToolErrorAvailable();
	BOOL IsUserInterface();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};
/////////////////////////////////////////////////////////////////////////////
// View_Dispatch wrapper class

class View_Dispatch : public COleDispatchDriver
{
public:
	View_Dispatch() {}		// Calls COleDispatchDriver default constructor
	View_Dispatch(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	View_Dispatch(const View_Dispatch& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL BringToFront();
	BOOL Close();
	BOOL IsExperimentView();
	BOOL IsView();
	BOOL IsDialog();
	BOOL IsUserInterface();
	long GetToolErrorCode();
	CString GetToolErrorMessage();
	BOOL IsAbstractDevice();
	BOOL IsExperiment();
	BOOL IsExperimentDataItem();
	BOOL IsExperimentDevice();
	BOOL IsExperimentElement();
	BOOL IsToolErrorAvailable();
	BOOL Equals(LPDISPATCH dispatch_hndl);
	long GetHashCode();
};