/*
 * MIT License
 *
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

import { TurboModule} from '@rnoh/react-native-openharmony/ts';
import { MMKV } from '@tencent/mmkv'
import { common } from '@kit.AbilityKit';
import  SecureKeystore  from './AssetStore'
import native from 'librnoh_mmkv_storage.so'

export type IndexType = 'stringIndex' | 'boolIndex' | 'numberIndex' | 'mapIndex' | 'arrayIndex';

export class MMKVNativeTurboModule extends TurboModule {
  public static NAME: string = "MMKVNative";
  private context = getContext(this) as common.UIAbilityContext; // UIAbilityContext
  private appContext = this.context.getApplicationContext();

  /**
   * 初始化MMKV  initializeMMKV
   * @return undefined
   */
  public initializeMMKV():string {
    let path = MMKV.initialize(this.appContext);
    return path;
  }

  /**
   * 创建MMKV实例
   * @param mmapID MMKV实例的唯一标识
   * @param mode MMKV实例的处理模式，默认为{@link MMKV.SINGLE_PROCESS_MODE}。
   * @param cryptKey MMKV实例的加密密钥（长度不超过16字节）。
   * @param rootPath MMKV实例的文件夹，默认为$(FilesDir)/mmkv。
   * @param indexing
   */
  public setupMMKVInstance(mmapID: string, mode?: number, cryptKey?: string, rootPath?: string,indexing?:boolean): boolean {
   let path =  MMKV.initialize(this.appContext);
   let result = native.setupMMKVInstance(mmapID,mode ?? MMKV.SINGLE_PROCESS_MODE,cryptKey,path,indexing);
   return result;
  }

  /**
   * 获取安全密钥
   * @param alias 密钥别名
   * @return value 安全密钥
   */
  public getSecureKey(alias: string): String | undefined {
    let Keystore = new SecureKeystore();
    let key:string|undefined = Keystore.getSecureKey(alias);
    return key
  }
  /**
   * 设置安全密钥
   * @param alias 密钥别名
   * @param key 密钥
   * @return boolean 是否设置成功
   */
  public setSecureKey(alias: string, key: string): boolean {
    let Keystore = new SecureKeystore();
    let result = Keystore.setSecureKey(alias,key);
    return result;
  }
  /**
   * 判断安全密钥是否存在
   * @param alias 密钥别名
   * @return boolean 是否存在
   */
  public secureKeyExists(alias: string): boolean {
    let Keystore = new SecureKeystore();
    let key:string|undefined = Keystore.getSecureKey(alias);
    if(key){
      return true
    }else{
      return false
    }
  }

  /**
   * 删除安全密钥
   * @param alias 密钥别名
   * @return value 安全密钥
   */
  public removeSecureKey(alias: string) {
    let Keystore = new SecureKeystore();
    Keystore.removeSecureKey(alias);
  }

  /**
   * 一组键值对设置到 MMKV 存储中
   * @param key: 键名
   * @param value: 键值
   * @param type: 类型
   * @param id: MMKV实例的唯一标识
   */
  public setMultiMMKV(key:string[],value:string[],type:string,id:string): boolean {
      let result = native.setMultiMMKV(key,value,type,id);
      return result;
  }

  /**
   * 获取MMKV实例的一组键值对
   * @param keys: 键的数组
   * @param id: mmkv实例的唯一标识
   */
  public getMultiMMKV(keys: string[], id: string): string[] {
    let result = native.getMultiMMKV(keys,id);
    return result;
  }

  /**
   * 一个字符串值设置到 MMKV 存储中 并且更新索引为stringIndex
   * @param key: 键
   * @param value: 值
   * @param id: mmkv实例的唯一标识
   * @returns true success 成功
   */
  public setStringMMKV(key: string, value: string, id: string): boolean | undefined {
   let result:boolean|undefined =  native.setStringMMKV(key,value,id);
   return result;
  }

  /**
   * 从 MMKV 实例中根据给定的键获取字符串值
   * @param key: 键
   * @param id: mmkv实例的唯一标识
   * @returns result 键值
   */
  public getStringMMKV(key: string, id: string): string | undefined {
   let result:string|undefined =  native.getStringMMKV(key,id)
   return result;

  }

  /**
   * 将一个键值对存储到 MMKV 实例中 并且更新索引为mapIndex
   * @param key: 键
   * @param value: 值
   * @param id: mmkv实例的唯一标识
   * @returns true success 成功
   */
  public setMapMMKV(key: string, value: string, id: string): boolean | undefined {
    let result:boolean|undefined =  native.setMapMMKV(key,value,id);
    return result;
  }

  /**
   * 从 MMKV 实例中根据给定的键获取字符串值
   * @param key: 键
   * @param id: mmkv实例的唯一标识
   * @returns result 键值
   */
  public getMapMMKV(key: string, id: string): string | undefined | null {
    let result:string|undefined =  native.getMapMMKV(key,id)
    return result;
  }

  /**
   * 将一个键值对存储到 MMKV 实例中 并且更新索引为arrayIndex
   * @param key: 键
   * @param value: 值
   * @param id: mmkv实例的唯一标识
   * @returns true success 成功
   */
  public setArrayMMKV(key: string, value: string, id: string): boolean | undefined {
    let result:boolean|undefined =  native.setArrayMMKV(key,value,id);
    return result;
  }

  /**
   * 从 MMKV 实例中根据给定的键获取字符串值
   * @param key: 键
   * @param id: mmkv实例的唯一标识
   * @returns result 键值
   */
  public getArrayMMKV(key: string, id: string): string | undefined | null {
    let result:string|undefined =  native.getArrayMMKV(key,id)
    return result;
  }

  /**
   * 将一个键值对存储到 MMKV 实例中 并且更新索引为 numberIndex
   * @param key: 键
   * @param value: 值
   * @param id: mmkv实例的唯一标识
   * @returns true success 成功
   */
  public setNumberMMKV(key: string, value: number, id: string): boolean | undefined {
    let result:boolean|undefined =  native.setNumberMMKV(key,value,id);
    return result;
  }

  /**
   * 从 MMKV 实例中根据给定的键获取字符串值
   * @param key: 键
   * @param id: mmkv实例的唯一标识
   * @returns result 键值
   */
  public getNumberMMKV(key: string, id: string): number | undefined | null {
    let result:number|undefined =  native.getNumberMMKV(key,id)
    return result;
  }

  /**
   * 将一个键值对存储到 MMKV 实例中 并且更新索引为booleanIndex
   * @param key: 键
   * @param value: 值
   * @param id: mmkv实例的唯一标识
   * @returns true success 成功
   */
  public setBoolMMKV(key: string, value: boolean, id: string): boolean | undefined {
    let result:boolean|undefined =  native.setBoolMMKV(key,value,id);
    return result;
  }

  /**
   * 从 MMKV 实例中根据给定的键获取字符串值
   * @param key: 键
   * @param id: mmkv实例的唯一标识
   * @returns result 键值
   */
  public getBoolMMKV(key: string, id: string): boolean | undefined | null {
    let result:boolean|undefined =  native.getBoolMMKV(key,id)
    return result;

  }

  /**
   * 从 MMKV 实例中根据给定的键删除对应的值
   * @param key: 键
   * @param id: mmkv实例的唯一标识
   * @returns true success 成功
   */
  public removeValueMMKV(key: string, id: string): boolean | undefined {
    let result:boolean|undefined =  native.removeValueMMKV(key,id)
    return result;
  }

  /**
   * 批量移除MMKV实例中的部分密钥。
   * @param keys: 键的数组
   * @param id: mmkv实例的唯一标识
   * @returns true success 成功
   */
  public removeValuesMMKV(keys: string[], id: string): boolean | undefined {
    let result:boolean|undefined =  native.removeValuesMMKV(keys,id);
    return result;
  }

  /**
   * 获得所有key
   * @param id: mmkv实例的唯一标识
   * @returns 所有keys值
   */
  public getAllKeysMMKV(id: string): string[] | undefined {
    let result:string[]|undefined  = native.getAllKeysMMKV(id);
    return result
  }

  /**
   * 从 MMKV 实例中获取指定键的所有索引
   * @param type: 索引名称
   * @param id: mmkv实例的唯一标识
   * @returns 所有指定键的索引
   */
  public getIndexMMKV(type: IndexType, id: string): string[] | undefined {
     let result:string[] | undefined = native.getIndexMMKV(type,id);
     return result;
  }

  /**
   * 该函数用于检查 MMKV 实例中是否包含指定的键
   * @param key: 键
   * @param id：mmkv实例的唯一标识
   * @returns 是否包含 true or false
   */
  public containsKeyMMKV(key: string, id: string): boolean | undefined {
    let result:boolean|undefined = native.containsKeyMMKV(key,id);
    return result
  }

  /**
   * 清空 MMKV 实例中的所有数据，并重新初始化其索引
   * @param id：mmkv实例的唯一标识
   * @returns true success 成功
   */
  public clearMMKV(id: string): boolean | undefined {
    let result:boolean|undefined = native.clearMMKV(id);
    return result;
  }

  /**
   * 清除MMKV实例的内存缓存
   * @param id：mmkv实例的唯一标识
   * @returns true success 成功
   */
  public clearMemoryCache(id: string): boolean | undefined {
    let result:boolean|undefined = native.clearMemoryCache(id);
    return result;
  }

  /**
   * 更新 MMKV 实例的加密密钥
   * @param cryptKey：需要加密的密钥
   * @param id：mmkv实例的唯一标识
   * @returns true success 成功
   */
  public encryptMMKV(cryptKey: string, id: string): boolean | undefined {
    let result:boolean|undefined = native.encryptMMKV(cryptKey,id);
    return result;
  }

  /**
   * 清除 MMKV 实例的加密密钥
   * @param id：mmkv实例的唯一标识
   * @returns true success 成功
   */
  public decryptMMKV(id: string): boolean | undefined {
    let result:boolean|undefined = native.decryptMMKV(id);
    return result;
  }
}