
#if !defined(AFX_VERSION_H_)
#define AFX_VERSION_H_

#define	D_VERSION_MAJOR		2
#define	D_VERSION_MINOR		2
#define	D_VERSION_PATCH		4
#define	D_VERSION_BUILD		0

#define	TOSTR(str) L#str
#define TOSTERVER(major, minor, pathc, build) TOSTR(major.minor.pathc.build)
#define D_FULL_VERSION TOSTERVER(D_VERSION_MAJOR, D_VERSION_MINOR, D_VERSION_PATCH, D_VERSION_BUILD)

#endif //AFX_VERSION_H_
