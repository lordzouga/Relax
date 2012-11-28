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
Image{
    id: root
    source: "qrc:/style/images/bg.png";
    anchors.centerIn: parent;
    property bool flipped: false

    signal startRefresh();
    signal stopRefresh();

    Connections{
        target: engine;
        onCopyStarted: {if(!flipped) flipped = true;}
        onFinalFinish: {if(flipped) flipped = false;}
    }

    Flipable{
        id: flipable
        height: refresh.height; width: refresh.width
        front: Image{id: refresh
            source: "qrc:style/images/editButton.png";
            MouseArea{anchors.fill: parent; onClicked: root.startRefresh();
                onPressed: {parent.height = 28; parent.width = 72}
                onReleased: {parent.width = 77; parent.height = 32}
        }
            Text {id: aText; text: "Refresh"; font.pointSize: 11; font.bold: true; color: "#FFFFFF"
            anchors.centerIn: parent}
    }
        back: Image{id: cancel
            source: "qrc:style/images/deleteButton.png";
            MouseArea{anchors.fill: parent; onClicked: root.stopRefresh();}
        }

        transform: Rotation{
            id: rotation; origin.x: flipable.width/2; origin.y: flipable.height/2;
            axis.x: 1; axis.y: 0; axis.z: 0; angle: 0
        }

        states: State{name: "flip"
            PropertyChanges{target: rotation; angle: 180;}
        when: root.flipped}

        transitions: Transition{
            NumberAnimation{target: rotation; property: "angle"; duration: 500}}
        }
}

