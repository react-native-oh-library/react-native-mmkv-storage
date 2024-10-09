import { asset } from '@kit.AssetStoreKit';
import { util } from '@kit.ArkTS';

class SecureKeystore {
  private stringToArray(str: string): Uint8Array {
    let textEncoder = new util.TextEncoder();
    return textEncoder.encodeInto(str);
  }

  public setSecureKey(alias: string, key: string): boolean {
    let attr: asset.AssetMap = new Map();
    attr.set(asset.Tag.SECRET, this.stringToArray(key));
    attr.set(asset.Tag.ALIAS, this.stringToArray(alias));
    attr.set(asset.Tag.IS_PERSISTENT, true);
    try {
      asset.addSync(attr);
      return true
    } catch (e) {
      return false
    }
  }

  public getSecureKey(alias: string): string | undefined {
    let query: asset.AssetMap = new Map();
    query.set(asset.Tag.ALIAS, this.stringToArray(alias));
    try {
      let res: Array<asset.AssetMap> = asset.querySync(query);
      let keyValue: string | undefined;
      res.forEach((value) => {
        let val = value.get(asset.Tag.ALIAS);
        keyValue = val?.toString()
      })
      return keyValue;
    } catch (e) {
      return undefined;
    }
  }

  //删除关键资产
  public removeSecureKey(alias: string) {
    let query: asset.AssetMap = new Map();
    query.set(asset.Tag.ALIAS, this.stringToArray(alias));
    try {
      asset.removeSync(query);
    } catch (e) {
    }
  }
}

export default SecureKeystore;