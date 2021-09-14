/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//import LogUtil from '../../../common/baseUtil/LogUtil.js';
//let logUtil = new LogUtil();
import prompt from '@system.prompt';
export default {
    props: {
    /**
         * Whether a single model is displayed or hidden
         */
        itemIsShow: {
            default: true,
        },
    /**
         * String value representing the Preference title
         */
        title: {
            default: '',
        },
    /**
         * String value representing the Preference summary
         */
        summary: {
            default: ''
        },
        iconList: {
            default: ''
        },
    /**
         * Drawable representing the Preference icon
         */
        icon: {
            default: ''
        },
    /**
         * Boolean indicating whether the user can interact with Preference
         * false:Preference will be grayed out and users cannot interact with it.
         * The default value is true
         */
        enabled: {
            default: true
        },
    /**
         * A boolean indicating whether the user can interact with the Preference.
         * The default value is true.
         */
        selectable: {
            default: true
        },
    /**
         * Whether the icon is displayed
         * The default value is false.
         */
        iconShow: {
            default: false
        },
        spaceShow: {
            default:false
        },
        isIconListShow: {
            default:false
        },
    /**
         * whether the Preference category or Preference category is visible
         * Does summary show or hide
         */
        isSummaryShow: {
            default: true
        },
    /**
         * Whether the text status is displayed
         * The default value is false
         */
        stateDescriptionShow: {
            default: false
        },
    /**
         * State description of a single model item
         */
        modelStateDescribe: {
            default: ''
        },
    /**
         * Entry item
         * Whether the icon at the end of a single model item is displayed
         * The default value is true
         */
        entranceToItemShow: {
            default: true
        },
    /**
         * The icon at the end of a single model item
         */
        entranceToItemIcon: {
            default: '/res/image/ic_permissions_arrow.png'
        },
    /**
         * Whether to display the switch item
         * The default value is false
         */
        switchesIsShow: {
            default: false
        },
    /**
         * switch item value
         * The default value is false
         */
        switchDefaultValue: {
            default: false
        },
    /**
         * item
         */
        item: {},
        itemType: {},
    /**
         * Switch item
         * monitor the changed value
         */
        switchOnChangeValue: {},
        showObject: {},
        itemFlag:true,
    /**
         * Icon behind title
         */
        headIcon: {
            default: '/res/image/head_icon.png'
        },
        isHeadIconShow: {
            default: false
        },
        dividerShow: {}
    },
    data() {
        return {
            showObj: this.showObject,
            itemData: this.item,
            itemType: this.itemType,
            itemFlag:this.itemFlag,
            switchDefaultValue: this.switchDefaultValue,
            switchOnChangeValue: this.switchOnChangeValue,
        };
    },
    action() {

        //        logUtil.info('permission entryComponent action start  this.itemData' + JSON.stringify(this.itemData));
        //        prompt.showToast({message:0});
        //        if(this.actionState==1) {

        this.$emit('actionType', {
            text: this.itemData,
            type: this.itemType,
            flag: this.itemFlag
        });
        this.itemFlag=!this.itemFlag
        //        }else{
        //            prompt.showToast({message:this.actionState});
        //        }
        //        logUtil.info("permission entryComponent action end");
    },
/**
     * The onchange event of the switch item
     */
    switchOnChange() {
        //        logUtil.info('permission entryComponent switchOnChange start switchChangeValue  this.switchOnChangeValue:' + this.switchOnChangeValue + ' | this.switchDefaultValue：' + this.switchDefaultValue);
        this.$emit('switchOnChangeType', {
            text: !this.switchOnChangeValue,
            defaultText: this.switchDefaultValue,
            detail: this.itemData,
        });
        this.switchOnChangeValue = !this.switchOnChangeValue;
        //        logUtil.info('permission entryComponent switchOnChange end');
    }
}