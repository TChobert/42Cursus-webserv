#pragma once

struct locationPropertiesFlags {

	bool rootSeen;
	bool allowedMethodsSeen;
	bool uploadAuthSeen;
	bool uploadDirSeen;
	bool autoIndexSeen;
	bool indexSeen;
	bool cgiSeen;
	bool returnSeen;
	bool maxBodySeen;

	locationPropertiesFlags(void);
	~locationPropertiesFlags(void);
	void resetFlags(void);
};
