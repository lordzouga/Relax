// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item{
    width: 450; height: 420;
    Image{id: background; source: "images/background.png"; anchors.fill: parent}
    Image{id: banner; x: -5; y: -60; source: "images/banner.png";
        Behavior on x{
            SequentialAnimation{
                PropertyAnimation{id: bannerAnimation; target: banner; property: "y"; to: -5;
                duration: 500; easing.type: Easing.OutCirc}
            PropertyAnimation{id: relaxAnimation;target: relax; properties: "opacity,scale"; to: 1.0;
                duration: 500; easing.type: Easing.OutCirc}
            ParallelAnimation{
                id: text1Animation
            PropertyAnimation{target: text1; property: "x"; to: 229; duration: 500
            easing.type: Easing.OutCirc}
            PropertyAnimation{target: text1; property: "opacity"; to: 1.0; duration: 500;
                easing.type: Easing.OutCirc}
            }
            ParallelAnimation{
                id: text2animation
            PropertyAnimation{ target: text2; property: "x"; to: 229; duration: 500
            easing.type: Easing.OutCirc}
            PropertyAnimation{target: text2; property: "opacity"; to: 1.0; duration: 500;
                easing.type: Easing.OutCirc}
            }
            ParallelAnimation{
                id: text3Animation
            PropertyAnimation{target: text3; property: "x"; to: 229; duration: 500
            easing.type: Easing.OutCirc}
            PropertyAnimation{target: text3; property: "opacity"; to: 1.0; duration: 500;
                easing.type: Easing.OutCirc}
            }
            PropertyAnimation{id: arrowAnimation; target: arrow; easing.amplitude: 1; easing.period: 1;
                properties: "opacity,scale"; to: 1.0; duration: 500; easing.type: Easing.OutCirc}
            }
        }
    }
    Image {id: relax; x: 16; y: 76; source: "images/header.png"; opacity: 0; scale: 0.5}
    Image{id: text1; x: 450; y: 133; source: "images/Text1.png"; opacity: 0}
    Image{id: text2; x: 450; y: 176; source: "images/Text2.png"; opacity: 0}
    Image{id: text3; x: 450; y: 222; source: "images/Text3.png"; opacity: 0}
    Image{id: arrow; x: 16; y: 330; source: "images/Export.png";opacity: 0; scale: 0.5}

    Component.onCompleted: { banner.x = -6}
}
