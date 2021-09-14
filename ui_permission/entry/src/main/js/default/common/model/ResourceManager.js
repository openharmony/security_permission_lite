import resmgr from '@ohos.resmgr';

const DEFAULT_ICON_URL = '../images/appicon.png';

export default class ResourceManager {
    ResourceManager() {
    }

    getAppIcon(path, bundleName) {
        return new Promise(resolve => {
            console.info("PermissionManager  ResourceManager getAppIcon start path = " + path + " bundleName = " + bundleName);
            if (path == null || path == undefined || path == "" ) {
                console.info("PermissionManager  ResourceManager defaultIcon callback ");
                resolve(DEFAULT_ICON_URL);
            } else {
                console.info("PermissionManager  ResourceManager resmgr.getResourceManager called");
                resmgr.getResourceManager(bundleName).then(item => {
                    console.info("PermissionManager  ResourceManager item.getMediaBase64 called");
                    item.getMediaBase64(path, (error, value) => {
                        console.info("PermissionManager  ResourceManager item.getMediaBase64 callback value = " + value);
                        if (value != null) {
                            resolve(value);
                        }
                    });
                });
            }
            console.info("PermissionManager  ResourceManager getAppIcon end");
        });
    }

    getAppName(labelId, bundleName, appName) {

        return new Promise(resolve => {
            console.info("PermissionManager  ResourceManager getAppName start labelId = " + labelId
            + " bundleName = " + bundleName + " appName = " + appName);
            if (labelId <= 0) {
                resolve(appName);
            } else {
                console.info("PermissionManager  ResourceManager resmgr.getResourceManager ");
                resmgr.getResourceManager(bundleName).then(item => {
                    console.info("PermissionManager  ResourceManager getAppName " + JSON.stringify(item));
                    item.getString(labelId, (error, value) => {
                        if (value != null) {
                            console.info("PermissionManager  ResourceManager  getAppName value = " + value);
                            resolve(value);
                        }
                    });
                });
            }
        });
    }
}