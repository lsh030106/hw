#include "stdafx.h"
#include "Userdb.h"


CUserdb::CUserdb(void)
{
}


CUserdb::~CUserdb(void)
{
}


BOOL CUserdb::id_dup(void)
{
	PGconn *pConn = NULL;
	pConn = Getcon();

	ConnStatusType status;
	status = PQstatus(pConn);



	return TRUE;
}


