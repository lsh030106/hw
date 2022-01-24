#pragma once
#include "stdafx.h"

class CUserdb
{
public:
	CUserdb(void);
	~CUserdb(void);
	BOOL id_dup(void);

private:
	PGconn * Getcon(void)
	{
		return PQconnectdb("user=test password=1234 dbname=user_info hostaddr=local port=5432");
	}

};

