import bundle from '@ohos.bundle';
import PinyinSort from '../utils/PinyinSort.js';
import Pinyin from '../utils/Pinyin.js';
import ResourceManager from './ResourceManager.js';

let mBundleInfoList = [];

export default class BundleInfoManager {
    BundleInfoManager() {
    }

    getBundleInfoList(){
        return this.mBundleInfoList;
    }

    clearBundleInfoList(){
        this.mBundleInfoList.length = 0;
    }

    updateBundleInfoList( callback ){
        bundle.getBundleInfos(1).then((data) => {
            console.info('**************************getBundleInfos start********************************');
            console.info('xxx getBundleInfos data length [' + data.length + ']');
            this.convertData( data, callback );
            console.info('**************************getBundleInfos end********************************');
        })
    }

    async convertData( data, callback ){
            var resourceManager = new ResourceManager();
            var pinyinSort = new PinyinSort();
            var pinyin = new Pinyin();
            var temp_array_app = [];
            this.mBundleInfoList = [];

            for( var index=0; index < data.length; index++ ){
                let item = data[index];
                if(!item.appInfo.systemApp){
                    var temp = {}
                    temp.bundleName = item.name;
    //                temp.permissionValue = item.reqPermissions.length + "项权限";
                    temp.reqPermissions = item.reqPermissions;
                    temp.permissionArrow = "/res/image/ic_permissions_arrow.png";
    //                temp.permissionDefaultValue = false;`
                    temp.permissionSummary = "";
                    temp.permissionShowDivider = true
    //                temp.permissionType = 1;
                    console.info('**************************get start********************************');
                    temp.permissionIcon = await resourceManager.getAppIcon(item.appInfo.iconId, item.name)
                    temp.appName = await resourceManager.getAppName(item.appInfo.labelId, item.name, item.appInfo.label)

                    temp.appName_pinyin = pinyin.getFullChars(temp.appName).toLowerCase();
                    temp_array_app.push(temp);
                    console.info('**************************get end********************************');

                }
            }
            this.mBundleInfoList.length = 0;
            this.mBundleInfoList = temp_array_app.slice();
            this.mBundleInfoList.sort(pinyinSort.sortByAppName.bind(pinyinSort));

            callback(this.mBundleInfoList );
    }

}