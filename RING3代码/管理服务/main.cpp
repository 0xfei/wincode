#include <windows.h>
#include <winsvc.h>
#include <stdio.h>

int main(int artc, char* argv[])
{
	int what;
	SC_HANDLE scm;
	SC_HANDLE service;
	char name[MAX_PATH];
	SERVICE_STATUS status;	
	if ( (scm=OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE))==NULL ) {
		printf("open scm error!\n");
		return 0;
	}
	printf("Enter 0 to delete a service, or to create a service.\n");
	scanf("%d", &what);
	if (what == 0) {
		printf("Enter the name of service to delete: ");
		scanf("%s", name);
		service = OpenService(scm, name, SERVICE_ALL_ACCESS|DELETE);
		if (!service) {
			printf("open service error!\n");
			return 0;
		}
		BOOL isSuccess = QueryServiceStatus(service, &status);
		if (!isSuccess) {
			printf("query information error!\n");
			return 0;
		}
		if (status.dwCurrentState != SERVICE_STOPPED) {
			isSuccess = ControlService(service, SERVICE_CONTROL_STOP, &status);
			if (!isSuccess) {
				printf("close service error!\n");
			}
			Sleep(500);
		}
		isSuccess = DeleteService(service);
		if (!isSuccess) printf("Delete service error!\n");
		else printf("Delete service success.\n");
		CloseServiceHandle(service);
		CloseServiceHandle(scm);
		return 0;
	} else {
		service = CreateService(scm, "test", "test", SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, 
			"C:\\Project\\VC\\服务启动\\Debug\\服务启动.exe", 
			0, 0, NULL, NULL, NULL);
		if (!service) {
			printf("create service error!\n");
		} else printf("Create service success.\n");
		StartService(service, 0, NULL);
		CloseServiceHandle(service);
		CloseServiceHandle(scm);
	}
	return 0;
}