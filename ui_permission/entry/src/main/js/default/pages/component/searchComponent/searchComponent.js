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
export default {
    props: {
        baseList: {
        },
        searchList: {
        },
        searchValue: {
            default: ''
        }
    },
    data() {

    },
    searchChange(e){
        var textLength = 500;
        this.searchValue = "";
        if(e.text.length >= textLength){
            e.text = e.text.substring(0,textLength);
        }
        this.searchValue = e.text;
        console.info("search text: " + e.text);
        this.baseList.forEach((item)=>{
            console.info("baseList : " + item.appName);
        })
        this.searchList = [];
        if(e.text == ''){
            this.searchList = this.baseList.slice();
        }else{
            this.baseList.forEach((item)=>{
                var reg = new RegExp("[\\u4E00-\\u9FFF]+","g");
                if(reg.test(e.text)){
                    console.info("是汉字");
                    if( -1 != item.appName.indexOf(e.text) ){
                        this.searchList.push(item);
                    }
                }else{
                    console.info("不是汉字");
                    if(-1 != item.appName_pinyin.indexOf(e.text.toLowerCase())){
                        this.searchList.push(item);
                    }
                }
            })
        }
        this.searchList.forEach((item)=>{
            console.info("this.searchList : " + item.appName);
        })
        if(this.searchList.length > 0){
            this.searchList[this.searchList.length -1].permissionShowDivider = false;
        }
        this.$emit('searchOnChange', {
            searchList: this.searchList,
        });
    }
}