/*
** NetXMS ODBCQUERY subagent - Simple ODBC API
** Copyright (C) 2004, 2005, 2006 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
**/

#include "odbcquery.h"

#ifndef _WIN32
#undef TCHAR
#undef WCHAR
#endif

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>


#define SQLRET_OK(c)		((c) == SQL_SUCCESS || (c) == SQL_SUCCESS_WITH_INFO)
#define SQLRET_FAIL(c)	(!SQLRET_OK(c))

#define MAX_STR			(256)
#define MAX_STMT			(1024)
#define TIMEOUT_LOGIN	(15)
#define TIMEOUT_CONN		(15)

#if defined(SUCCESS)
#undef SUCCESS
#endif
#define SUCCESS	(0)
#if defined(FAIL)
#undef FAIL
#endif
#define FAIL		(-1)

#define 	MSG_OK			("OK")
#define 	MSG_ECTX		 	("Invalid ODBC context")
#define	MSG_EHENV	 	("Failed to allocate env handle")
#define	MSG_ECONNH	 	("Failed to allocate connection handle")
#define	MSG_EVERSION 	("Failed to set ODBC version")
#define	MSG_ECONNECT 	("Failed to connect to database")
#define	MSG_EHSTMT		("Failed to allocate statement handle")
#define	MSG_LOGINTOUT 	("Failed to set login timeout")
#define	MSG_CONNTOUT 	("Failed to set comms DB timeout")
#define  MSG_EDISCONN	("Disconnect failed")
#define	MSG_EBIND		("Error binding result column")
#define  MSG_EFETCH		("Error fetching SQL result")
#define  MSG_EEXECSQL	("Error executing SQL statement")
#define  MSG_ENOTFOUND	("No data found")

typedef struct SqlCtx {
	TCHAR		szOdbcMsg[MAX_STR];	/* API error message */
	SQLCHAR	szSqlErr[MAX_STR];	/* sql error message */
	SQLCHAR	szSqlStat[10];	/* sql op status */
	SQLINTEGER	nSqlErr;		/* native sql error code */
	SQLSMALLINT	nLen;			/* length of szSqlErr */
	SQLHENV		hEnv;			/* ODBC environment handle */
	SQLHDBC		hDbc;			/* ODBC connection handle */
	SQLHSTMT		hStmt;		/* ODBC current statement handle */
	TCHAR		szSqlStmt[MAX_STMT];	/* last SQL statement */
} SqlCtx;

//
// ODBC functions
//

void* OdbcCtxAlloc(void)
{
	SqlCtx* pSqlCtx;

	if (pSqlCtx = (SqlCtx*)malloc(sizeof(SqlCtx)))
	{
		memset(pSqlCtx, 0, sizeof(SqlCtx));
		pSqlCtx->szSqlStmt[0] = _T('\0');
		pSqlCtx->hStmt = 0;
	}

	return (void*)pSqlCtx;
}

void OdbcCtxFree(void* pvSqlCtx)
{
	SqlCtx* pSqlCtx = (SqlCtx*)pvSqlCtx;

	if (pvSqlCtx)
	{
		if (pSqlCtx->hStmt != NULL)
			SQLFreeHandle(SQL_HANDLE_STMT, pSqlCtx->hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, pSqlCtx->hDbc);
		SQLFreeHandle(SQL_HANDLE_ENV, pSqlCtx->hEnv);
		free(pvSqlCtx);
	}

	return;
}

int OdbcConnect(void* pvSqlCtx, const TCHAR* pszSrc)
{
	int nRet = SUCCESS, nSqlRet;
	SqlCtx* pSqlCtx = (SqlCtx*)pvSqlCtx;

	if (nRet == SUCCESS && pvSqlCtx == NULL)
	{
		_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_ECTX));
		nRet = FAIL;
	}

	if (nRet == SUCCESS)
	{
		nSqlRet = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &pSqlCtx->hEnv);
		if (SQLRET_FAIL(nSqlRet))
		{
			_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_EHENV));
			nRet = FAIL;
		}
	}

	if (nRet == SUCCESS)
	{
		nSqlRet = SQLSetEnvAttr(pSqlCtx->hEnv, SQL_ATTR_ODBC_VERSION, 
										(void*)SQL_OV_ODBC3, 0);
		if (SQLRET_FAIL(nSqlRet))
		{
			_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_EVERSION));
			nRet = FAIL;
		}
	}

	if (nRet == SUCCESS)
	{
		nSqlRet = SQLAllocHandle(SQL_HANDLE_DBC, pSqlCtx->hEnv, &pSqlCtx->hDbc);
		if (SQLRET_FAIL(nSqlRet))
		{
			_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_ECONNH));
			nRet = FAIL;
		}
	}

   if (nRet == SUCCESS)
   {
		nSqlRet = SQLSetConnectAttr(pSqlCtx->hDbc, SQL_ATTR_LOGIN_TIMEOUT,
		                            (SQLPOINTER*)TIMEOUT_LOGIN, 0);
		if (SQLRET_FAIL(nSqlRet))
		{
			_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_LOGINTOUT));
			nRet = FAIL;
		}
	}

   if (nRet == SUCCESS)
   {
		nSqlRet = SQLSetConnectAttr(pSqlCtx->hDbc, SQL_ATTR_CONNECTION_TIMEOUT,
		                            (SQLPOINTER*)TIMEOUT_CONN, 0);
		if (SQLRET_FAIL(nSqlRet))
		{
			_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_CONNTOUT));
			 nRet = FAIL;
		}
	}

	if (nRet == SUCCESS)
	{
		nSqlRet = SQLConnect(pSqlCtx->hDbc, (SQLCHAR*)pszSrc, SQL_NTS, NULL,
									SQL_NTS, NULL, SQL_NTS);
		if (SQLRET_FAIL(nSqlRet))
		{
			_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_ECONNECT));
			SQLGetDiagRec(SQL_HANDLE_DBC, pSqlCtx->hDbc, 1, pSqlCtx->szSqlStat,
							  &pSqlCtx->nSqlErr, pSqlCtx->szSqlErr, MAX_STR, 
							  &pSqlCtx->nLen);
			nRet = FAIL;
		}
	}

	if (nRet == SUCCESS)
		_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_OK));

	return nRet;	
}

int OdbcDisconnect(void* pvSqlCtx)
{
	int nRet = SUCCESS;
	SQLRETURN nSqlRet;
	SqlCtx* pSqlCtx = (SqlCtx*)pvSqlCtx;

	if (nRet == SUCCESS && pvSqlCtx == NULL)
	{
		_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_ECTX));
		nRet = FAIL;
	}

	if (nRet == SUCCESS)
	{
		if (pSqlCtx->hStmt != NULL)
		{
			SQLFreeHandle(SQL_HANDLE_STMT, pSqlCtx->hStmt);
			pSqlCtx->hStmt = NULL;
		}
		nSqlRet = SQLDisconnect(pSqlCtx->hDbc);
	}

	if (SQLRET_FAIL(nSqlRet))
	{
		_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_EDISCONN));
		nRet = FAIL;
	}

	if (nRet == SUCCESS)
		_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_OK));

	return nRet;
}

int OdbcQuerySelect1(void* pvSqlCtx, const TCHAR* pszQuery, 
							TCHAR* pszResult, size_t nResSize)
{
	int nRet = SUCCESS;
	SQLRETURN nSqlRet;
	SQLLEN nLen;
	TCHAR	szBuf[MAX_STR];
	SqlCtx* pSqlCtx = (SqlCtx*)pvSqlCtx;
	BOOL bSame = TRUE, bCursorOpened = FALSE;

	*pszResult = 0;

	if (nRet == SUCCESS && _tcsncmp(pSqlCtx->szSqlStmt, pszQuery, MAX_STMT))
	{
		bSame = FALSE;
		_tcsncpy(pSqlCtx->szSqlStmt, pszQuery, MAX_STMT);
		pSqlCtx->szSqlStmt[MAX_STMT - 1] = _T('\0');
	}

	if (nRet == SUCCESS && !bSame)
	{
		if (pSqlCtx->hStmt != NULL)
		{
			SQLFreeHandle(SQL_HANDLE_STMT, pSqlCtx->hStmt);
			pSqlCtx->hStmt = NULL;
		}
		nSqlRet = SQLAllocHandle(SQL_HANDLE_STMT, pSqlCtx->hDbc, &pSqlCtx->hStmt);
		if (SQLRET_FAIL(nSqlRet))
		{
			_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_EHSTMT));
			nRet = FAIL;
		}
	}

	if (nRet == SUCCESS)
	{
		nSqlRet = SQLBindCol(pSqlCtx->hStmt, 1, SQL_C_CHAR, szBuf, 
									MAX_STR, &nLen);
		pSqlCtx->nSqlErr = (SQLINTEGER)nLen;
		if (SQLRET_FAIL(nSqlRet))
		{
			_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_EBIND));
			nRet = FAIL;
		}
	}

	if (nRet == SUCCESS)
	{
		nSqlRet = SQLExecDirect(pSqlCtx->hStmt, (SQLCHAR*)pszQuery, SQL_NTS);
		if (SQLRET_FAIL(nSqlRet))
		{
			_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_EEXECSQL));
			SQLGetDiagRec(SQL_HANDLE_STMT, pSqlCtx->hStmt, 1, pSqlCtx->szSqlStat,
							  &pSqlCtx->nSqlErr, pSqlCtx->szSqlErr, MAX_STR, 
							  &pSqlCtx->nLen);
			nRet = FAIL;
		}
	}

	if (nRet == SUCCESS)
	{
		bCursorOpened = TRUE;
		nSqlRet = SQLFetch(pSqlCtx->hStmt);
		if (nSqlRet != SQL_NO_DATA && SQLRET_FAIL(nSqlRet))
		{
			_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_EFETCH));
			SQLGetDiagRec(SQL_HANDLE_STMT, pSqlCtx->hStmt, 1, pSqlCtx->szSqlStat,
							  &pSqlCtx->nSqlErr, pSqlCtx->szSqlErr, MAX_STR, 
							  &pSqlCtx->nLen);
			nRet = FAIL;
		}
		else if (nSqlRet == SQL_NO_DATA)
		{
			_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_ENOTFOUND));
			nRet = FAIL;
		}
	}

	if (bCursorOpened)
		SQLCloseCursor(pSqlCtx->hStmt);

	if (nRet == SUCCESS)
	{
		_tcsncpy(pszResult, szBuf, MAX_STR);
		_tcscpy(pSqlCtx->szOdbcMsg, _T(MSG_OK));
	}

	return nRet;
}

TCHAR* OdbcGetInfo(void* pvSqlCtx)
{
	SqlCtx* pSqlCtx = (SqlCtx*)pvSqlCtx;

	return pSqlCtx->szOdbcMsg;
}

TCHAR* OdbcGetSqlError(void* pvSqlCtx)
{
	SqlCtx* pSqlCtx = (SqlCtx*)pvSqlCtx;

	return (TCHAR*)pSqlCtx->szSqlErr;
}

int OdbcGetSqlErrorNumber(void* pvSqlCtx)
{
	SqlCtx* pSqlCtx = (SqlCtx*)pvSqlCtx;

	return (int)pSqlCtx->nSqlErr;
}
