#pragma once 
#include <iostream> 
#include <list> 
#include "sqlite3.h" 
static const std::wstring QUERY_TABLE_CREATE  = L"CREATE TABLE TEST (DeviceType	TEXT NOT NULL,Date	TEXT,Type	TEXT,Content	TEXT);" ; 
static const std::wstring QUERY_TABLE_EXIST = L"SELECT name FROM sqlite_master WHERE type='table' AND name='TEST';"; 
static const std::wstring QUERY_TABLE_DROP = L"DROP TABLE IF EXISTS TEST;"; 
static const std::wstring QUERY_TABLE_EXIST_RECORD = L"SELECT FilePath FROM TEST WHERE FilePath=? AND CheckSum=? AND Version=?;"; 
static const std::wstring QUERY_TABLE_DELETE_RECORD = L"DELETE FROM TEST WHERE FilePath=?;"; 
static const std::wstring QUERY_TABLE_UPSERT_RECORD = L"INSERT INTO TEST VALUES(?, ?, ?, ?);"; 
static const std::wstring QUERY_TABLE_SELECT_ALL = L"SELECT FilePath FROM TEST;"; 
static const std::wstring QUERY_DB_CLEAN_UP = L"VACUUM;"; 
static const std::wstring QUERY_DB_BEGIN = L"BEGIN;"; 
static const std::wstring QUERY_DB_COMMIT = L"COMMIT;"; 
static const std::wstring QUERY_DB_ROLLBACK     = L"ROLLBACK;"; 

class __declspec(dllexport) fileDB
{
private:    
	sqlite3 *db;

public:
	static fileDB* GetInstance()
	{
		if (instance == NULL) instance = new fileDB();
		return instance;
	}
	bool Open(std::wstring dbFileName);
	bool Close();  
	bool Upsert(std::string deviceType, std::string type, std::string content);
	bool Delete(std::wstring key);  std::list<std::wstring> GetAllRecords();

private: 
	static fileDB* instance;

	bool DropTable();   
	bool CreateTable(); 
	bool IsTableExist(); 

private:    
	bool Begin();   
	bool Commit();  
	bool Rollback(); 
};
