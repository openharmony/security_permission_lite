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
import router from '@system.router';

export default {
    props: {
        titleName: {
            default: '',
        },
        backIcon: {
            default: '/res/image/ic_back.png',
        },
        backIsShow: {
            default: true,
        },
        buttonStyles: {
            default: 'app-bar-image'
        },
        fontSize:{
            default:"24px"
        },
        moreIsShow: {
            default: false,
        },
    },
    onShow() {
        this.$element('backId').focus()
    },
    back() {
        console.info('permission headComponent back start');
        this.$emit('backType', {});
        console.info('permission headComponent back end');
    },
    backChangeBackground() {
        console.info('permission headComponent backChangeBackground start');
        this.buttonStyles = 'app-bar-image-change-background'
        console.info('permission headComponent backChangeBackground end');
    },
    backRouter() {
        console.info('permission headComponent backRouter start');
        router.back();
        console.info('permission headComponent backRouter end');
    },
    onMenuSelected(event){
        if( event.value === "permission_record"){
            router.push ({
                uri:"pages/distributedPermissionRecords/distributedPermissionRecords",
            })
        }
    }
}

