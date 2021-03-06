// SqliteTest.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "DB.h"
#include <atlstr.h>


char* ANSIToUTF8(const char * pszCode) {
	int     nLength, nLength2;
	BSTR    bstrCode;
	char*   pszUTFCode = NULL;

	nLength = MultiByteToWideChar(CP_ACP, 0, pszCode, lstrlenA(pszCode), NULL, NULL);
	bstrCode = SysAllocStringLen(NULL, nLength);
	MultiByteToWideChar(CP_ACP, 0, pszCode, lstrlenA(pszCode), bstrCode, nLength);


	nLength2 = WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, pszUTFCode, 0, NULL, NULL);
	pszUTFCode = (char*)malloc(nLength2 + 1);
	WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, pszUTFCode, nLength2, NULL, NULL);
	SysFreeString(bstrCode);

	return pszUTFCode;
}
int main()
{
	char * str = ANSIToUTF8("로그인 성공");
	fileDB::GetInstance()->Open(L"test.db");
	fileDB::GetInstance()->Upsert("Client", "INFO",str);


    return 0;
}

