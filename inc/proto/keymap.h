struct InputEvent {
	int ie_Class;
	int ie_SubClass;
	int ie_Code;
	int ie_Qualifier;
	int ie_Prev1DownCode;
	int ie_Prev1DownQual;
	int ie_Prev2DownCode;
	int ie_Prev2DownQual;
};

#define IECLASS_RAWKEY 0
#define IEQUALIFIER_LALT 1
#define IEQUALIFIER_LSHIFT 2

static WORD MapRawKey()
{
	// FIXME ACE 1.9 implement MapRawKey?
	return 0;
}
