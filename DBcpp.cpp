#include <stdio.h> 
#include "sqlite3.h" 
#include "DB.h" 
//### public ###

#include <iostream>
#include <chrono>
#include <ctime>
#include "ConvertUTF8.h"

std::string getTimeStr() {
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::string s(30, '\0');
	std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	return s;
}

fileDB* fileDB::instance = nullptr;

bool fileDB::Open(std::wstring dbFileName)
{ 
	int rc = sqlite3_open16(dbFileName.c_str(), &db);  
	if(rc != SQLITE_OK) 
	{ 
		sqlite3_close(db); 
		return false;   
	}   
	// table check  
	if(this->IsTableExist() == false) 
	{ 
		this->CreateTable();
	}   
	return true; 
} 

bool fileDB::Close() 
{   
	return (sqlite3_close(db) == SQLITE_OK); 
} 

bool fileDB::Upsert(std::string deviceType, std::string type, std::string content) 
{ 
	
	sqlite3_stmt * stmt;    
	const wchar_t * query = QUERY_TABLE_UPSERT_RECORD.c_str();  
	bool result = true; 
	
	sqlite3_prepare16_v2(this->db, query, -1, &stmt, NULL);  
	sqlite3_bind_text(stmt, 1, deviceType.c_str() , -1,SQLITE_STATIC);   
	sqlite3_bind_text(stmt, 2, getTimeStr().c_str(), -1,SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, type.c_str(), -1, SQLITE_STATIC);
	if(content.c_str() == NULL) 
	{        
		sqlite3_bind_text(stmt, 4, NULL, -1,SQLITE_STATIC);    
	} 
	else 
	{       
	
		sqlite3_bind_text(stmt, 4, content.c_str(), content.size(), SQLITE_STATIC);
	}   
	
	// begin    
	this->Begin();   
	if(sqlite3_step(stmt) != SQLITE_DONE) 
	{ 
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(this->db)); 
		result = false; 
	}  
	
	// commit   
	this->Commit(); 
	sqlite3_reset(stmt);    
	sqlite3_finalize(stmt); 
	
	return result; 
} 

bool fileDB::Delete(std::wstring filePath) 
{ 
	sqlite3_stmt * stmt;    
	const wchar_t * query = QUERY_TABLE_DELETE_RECORD.c_str();
	bool result = true; 
	sqlite3_prepare16_v2(this->db, query, -1, &stmt, NULL); 
	sqlite3_bind_text16(stmt, 1, filePath.c_str(), -1,SQLITE_STATIC);      
	
	// begin    
	this->Begin();   
	if(sqlite3_step(stmt) != SQLITE_DONE) 
	{ 
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(this->db)); 
		result = false; 
	}  
	
	// commit   
	this->Commit();  
	sqlite3_reset(stmt);    
	sqlite3_finalize(stmt); 
	
	return result; 

} 

std::list<std::wstring> fileDB::GetAllRecords() 
{    
	std::list<std::wstring> result;  
	sqlite3_stmt * stmt;    
	const wchar_t * query = QUERY_TABLE_SELECT_ALL.c_str(); 
	sqlite3_prepare16_v2(this->db, query, -1, &stmt, NULL);  
	while(sqlite3_step(stmt) == SQLITE_ROW) 
	{ 
		int type = sqlite3_column_type(stmt, 0);
		if(type != SQLITE_TEXT) 
		{ 
			continue; 
		}       
		std::wstring fileName((wchar_t *)sqlite3_column_text16(stmt, 0));       
		result.push_back(fileName); 
	}   
	sqlite3_reset(stmt);    
	sqlite3_finalize(stmt); 
	return result; 
} 


//### private ###
bool fileDB::DropTable() 
{    
	sqlite3_stmt * stmt;    
	const wchar_t * query = QUERY_TABLE_DROP.c_str();  
	bool result = true; sqlite3_prepare16_v2(this->db, query, -1, &stmt, NULL);

	// begin    
	this->Begin();   
	if(sqlite3_step(stmt) != SQLITE_DONE) 
	{ 
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(this->db)); 
		result = false; 
	}  

	// commit  
	this->Commit();      
	sqlite3_reset(stmt);    
	sqlite3_finalize(stmt); 
	return result; 
}

bool fileDB::CreateTable()
{   
	sqlite3_stmt * stmt;    
	const wchar_t * query = QUERY_TABLE_CREATE.c_str(); 
	bool result = true; 
	sqlite3_prepare16_v2(this->db, query, -1, &stmt, NULL); 
	
	// begin    
	this->Begin();   
	if(sqlite3_step(stmt) != SQLITE_DONE) 
	{ 
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(this->db)); 
		result = false;
	}  
	
	// commit   
	this->Commit();  
	sqlite3_reset(stmt);    
	sqlite3_finalize(stmt); 
	return result; 
} 

bool fileDB::IsTableExist()
{  
	sqlite3_stmt * stmt;    
	const wchar_t * query = QUERY_TABLE_EXIST.c_str();  
	bool result = false;    
	sqlite3_prepare16_v2(this->db, query, -1, &stmt, NULL); 
	if(sqlite3_step(stmt) == SQLITE_ROW) 
	{ 
		result = true; 
	} 
	sqlite3_reset(stmt);    
	sqlite3_finalize(stmt); 
	return result; 
} 

bool fileDB::Begin()
{ 
	sqlite3_stmt * stmt;    
	const wchar_t * query = QUERY_DB_BEGIN.c_str(); 
	bool result = true; 
	sqlite3_prepare16_v2(this->db, query, -1, &stmt, NULL);  

	if(sqlite3_step(stmt) != SQLITE_DONE) 
	{ 
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(this->db)); 
		result = false; 
	}  
	sqlite3_reset(stmt);    
	sqlite3_finalize(stmt); 
	
	return result; 
} 

bool fileDB::Commit()
{    
	sqlite3_stmt * stmt;    
	const wchar_t * query = QUERY_DB_COMMIT.c_str();    
	bool result = true; 
	sqlite3_prepare16_v2(this->db, query, -1, &stmt, NULL);  

	if(sqlite3_step(stmt) != SQLITE_DONE) 
	{ 
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(this->db));
		result = false;
	}  
	sqlite3_reset(stmt);    
	sqlite3_finalize(stmt); 
	
	return result; 
} 

bool fileDB::Rollback()
{  
	sqlite3_stmt * stmt;    
	const wchar_t * query = QUERY_DB_ROLLBACK.c_str(); 
	bool result = true; 
	sqlite3_prepare16_v2(this->db, query, -1, &stmt, NULL);  
	if(sqlite3_step(stmt) != SQLITE_DONE) 
	{ 
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(this->db)); 
		result = false; 
	}  
	
	sqlite3_reset(stmt);    
	sqlite3_finalize(stmt); 
	return result; 
}
