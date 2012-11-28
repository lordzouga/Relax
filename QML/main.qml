/*
Copyright: 2012 LORDZOUGA <ozojiechikelu@gmail.com>
License: GPL-2+
 This package is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 .
 This package is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 .
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>

*/
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
    Image {id: relax; x: 16; y: 76; source: "qrc:/images/header.png"; opacity: 0; scale: 0.5}
    Image{id: text1; x: 450; y: 133; source: "qrc:/images/Text1.png"; opacity: 0}
    Image{id: text2; x: 450; y: 176; source: "qrc:/images/Text2.png"; opacity: 0}
    Image{id: text3; x: 450; y: 222; source: "qrc:/images/Text3.png"; opacity: 0}
    Image{id: arrow; x: 16; y: 330; source: "qrc:/images/Export.png";opacity: 0; scale: 0.5}

    Component.onCompleted: { banner.x = -6}
}
