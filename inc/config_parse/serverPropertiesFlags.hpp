#pragma once

struct serverPropertiesFlags {

	bool portSeen;
	bool hostSeen;
	bool nameSeen;
	bool rootSeen;

	serverPropertiesFlags(void);
	~serverPropertiesFlags(void);
	void resetFlags(void);
};
