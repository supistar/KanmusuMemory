/*
 * Copyright 2013 KanMemo Project.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import QtQuick 2.0

Rectangle {
    id: root
//    color: "black"

    property int mode: 0    //0:どろー, 1:トリミング, 2:エフェクト

    property string tempPath: ""
    property string tempPathNP: ""
    property string editPath: ""

    property rect cursor: Qt.rect(clip.cursor.x - image.x, clip.cursor.y - image.y, clip.cursor.width, clip.cursor.height)

    Component.onCompleted: {
        root.cursor = Qt.rect(0, 0, image.width, image.height)

        canvas.loadImageUrl = root.tempPath
        canvas.loadImage(canvas.loadImageUrl)
    }

    Image {
        id: image
        anchors.centerIn: parent
        source: root.tempPath
//        visible: false
        opacity: 0.2
    }

    Canvas {
        id: canvas
        anchors.fill: image

        renderTarget: Canvas.Image          //描画先を指定
        renderStrategy: Canvas.Immediate    //描画スレッドの指定
        antialiasing: true                  //描画を滑らかにする

        property string loadImageUrl: ""    //読み込み用

        //画像の読み込み完了
        onImageLoaded: requestPaint()

        //描画処理
        onPaint: {
            var ctx = canvas.getContext("2d")
            //状態保存
            ctx.save()
            //クリア
            ctx.clearRect(0, 0, canvas.width, canvas.height)
            //画像の描画
            if(isImageLoaded(loadImageUrl)){
                ctx.drawImage(loadImageUrl, root.cursor.x, root.cursor.y, root.cursor.width, root.cursor.height
                              , canvas.width / 2 - root.cursor.width / 2, canvas.height / 2 - root.cursor.height / 2
                              , root.cursor.width, root.cursor.height)
            }

            //状態復帰
            ctx.restore()
        }
    }

    //トリミングのカーソル
    ClipTool {
        id: clip
        anchors.fill: parent
        visible: false

        property rect oldCursor: Qt.rect(0, 0, 0, 0)
        onVisibleChanged: {
            if(visible){
                //前の状態を保存
                oldCursor = root.cursor
                //初期位置を決める
                var x = canvas.width / 2 - root.cursor.width / 2 + image.x
                var y = canvas.height / 2 - root.cursor.height / 2 + image.y
                var width = root.cursor.width
                var height = root.cursor.height
                if(root.cursor.width > 400 && root.cursor.height > 400){
                    clip.cursor = Qt.rect(x + 100, y + 100, width - 200, height - 200)
                }else{
                    clip.cursor = Qt.rect(x, y, width, height)
                }
                availableRange = Qt.rect(x, y, width, height)

                console.debug("load:" + clip.cursor)
            }
        }

        onPositionChanged: canvas.requestPaint()

        onAccepted: {
            root.cursor = Qt.rect(clip.cursor.x - image.x, clip.cursor.y - image.y, clip.cursor.width, clip.cursor.height)
            canvas.requestPaint()
            root.mode = 0
            console.debug("save:" + root.cursor)
        }
        onCanceled: {
            //クリップ位置をリセット
            root.cursor = root.cursor
            canvas.requestPaint()
            root.mode = 0
        }
    }

//    ClipCursor {
//        id: clip
//        anchors.fill: parent
//        visible: false

//        cursor: Qt.rect(image.x + 100, image.y + 100, image.width - 200, image.height - 200)
//        availableRange: Qt.rect(image.x, image.y, image.width, image.height)

//        MouseArea {
//            property point prev: "0,0"
//            anchors.fill: parent
//            onPressed: {
//                prev.x = mouse.x
//                prev.y = mouse.y
//            }
//            onPositionChanged: {
//                clip.cursor.x += mouse.x - prev.x
//                clip.cursor.y += mouse.y - prev.y
//                prev.x = mouse.x
//                prev.y = mouse.y

//                canvas.requestPaint()
//            }
//            onReleased: {
//                clip.cursor.x += mouse.x - prev.x
//                clip.cursor.y += mouse.y - prev.y

//                canvas.requestPaint()
//            }
//        }

//        property rect oldCursor: Qt.rect(0, 0, 0, 0)
//        onVisibleChanged: {
//            if(visible){
//                //前の状態を保存
//                oldCursor = root.cursor
//                //初期位置を決める
//                var x = canvas.width / 2 - root.cursor.width / 2 + image.x
//                var y = canvas.height / 2 - root.cursor.height / 2 + image.y
//                var width = root.cursor.width
//                var height = root.cursor.height
//                if(root.cursor.width > 400 && root.cursor.height > 400){
//                    clip.cursor = Qt.rect(x + 100, y + 100, width - 200, height - 200)
//                }else{
//                    clip.cursor = Qt.rect(x, y, width, height)
//                }
//                availableRange = Qt.rect(x, y, width, height)

//                console.debug("load:" + clip.cursor)
//            }
//        }

//        Row {
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.bottom: parent.bottom
//            anchors.bottomMargin: 5

//            spacing: 5
//            Button {
//                text: "Apply"
//                onClicked: {
//                    root.cursor = Qt.rect(clip.cursor.x - image.x, clip.cursor.y - image.y, clip.cursor.width, clip.cursor.height)
//                    canvas.requestPaint()
//                    root.mode = 0
//                    console.debug("save:" + root.cursor)
//                }
//            }
//            Button {
//                text: "Cancel"
//                onClicked: {
//                    //クリップ位置をリセット
//                    root.cursor = root.cursor
//                    canvas.requestPaint()
//                    root.mode = 0
//                }
//            }
//        }
//    }


    states: [
        State { // トリミング
            when: root.mode == 1
            PropertyChanges {
                target: clip
                visible: true
            }
        }
        , State {   //エフェクト
            when: root.mode == 2
        }

    ]
}
