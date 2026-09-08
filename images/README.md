# 图片资源

第一版保留车辆、地图和摄像头画面的文字占位，不包含真实地图或摄像头数据。
后续将独立素材放在此目录，并加入 resources.qrc 的 /images 前缀。
在 Designer 中为 labelCarImage、labelMapPreview、labelCameraPreview 选择 pixmap 后清空 text；保持 scaledContents 关闭，提前按区域尺寸等比例处理图片。
