VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form Form1 
   AutoRedraw      =   -1  'True
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Form1"
   ClientHeight    =   5100
   ClientLeft      =   45
   ClientTop       =   390
   ClientWidth     =   3585
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   255
   ScaleMode       =   2  'Point
   ScaleWidth      =   179.25
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command1 
      Caption         =   "décode GIF"
      Height          =   495
      Left            =   840
      TabIndex        =   2
      Top             =   3720
      Width           =   1695
   End
   Begin VB.CommandButton Command2 
      Caption         =   "Picture ->  .h"
      Height          =   495
      Left            =   840
      TabIndex        =   1
      Top             =   3000
      Width           =   1695
   End
   Begin MSComDlg.CommonDialog CommonDlg 
      Left            =   2160
      Top             =   1200
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.PictureBox pict 
      Appearance      =   0  'Flat
      AutoRedraw      =   -1  'True
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      ForeColor       =   &H80000008&
      Height          =   2640
      Left            =   120
      ScaleHeight     =   174
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   198
      TabIndex        =   0
      Top             =   120
      Width           =   3000
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
 
Private Sub Command1_Click()
Dim xh%, xl%, nw%
Dim xx%, oc%, nxx As Long
Dim s$, f$, f2$

'Ouvre le fichier gpx entrée en lecture
CommonDlg.ShowOpen

f$ = CommonDlg.FileName
f2$ = Replace(f$, ".", "") + ".h"

'Ouvre le fichier .h en écriture
Set logfilobj = CreateObject("Scripting.FileSystemObject")
Set logfile = logfilobj.CreateTextFile(f2$, True)
logfile.WriteLine ("uint8_t bmp[]={")
 
'Ouvre le fichier histo en lecture
Open (f$) For Binary As #1
'Boucle lecture fichier - Lit sans arret & Arrete tout si fin de fichier !
oc = 0
Do
  Get #1, , xx
  oc = oc + 2
  If oc > 1020 Then
    oc = oc
  End If
  If xx < 0 Then
    nxx = 65536 + xx
  Else
    nxx = xx
  End If
  xl = (nxx And 255)
  xh = (nxx - xl) / 256
  logfile.Write ("0x" + Hex$(xl) + "," + "0x" + Hex$(xh))
  
  If EOF(1) Then Exit Do
  'sinon ,
  logfile.Write (",")
   
  nw = nw + 1
  If nw > 7 Then
    logfile.Write (Chr$(13))
    nw = 0
  End If
  
Loop
Close #1

logfile.Write ("};")
logfile.Close
 
MsgBox f$ + " codé en " + f2$
Unload Me
End Sub

Private Sub Command2_Click()
Dim xh%, xl%, nw%
Dim xx%, oc%, nxx As Long
Dim s$, f$, f2$

'Ouvre le fichier gpx entrée en lecture
CommonDlg.ShowOpen

f$ = CommonDlg.FileName
f2$ = Replace(f$, ".", "") + ".h"

'Ouvre le fichier .h en écriture
Set logfilobj = CreateObject("Scripting.FileSystemObject")
Set logfile = logfilobj.CreateTextFile(f2$, True)
logfile.WriteLine ("uint8_t bmp[]={")
 
'Ouvre le fichier histo en lecture
Open (f$) For Binary As #1
'Boucle lecture fichier - Lit sans arret & Arrete tout si fin de fichier !
oc = 0
Do
  Get #1, , xx
  oc = oc + 2
  If oc > 1020 Then
    oc = oc
  End If
  If xx < 0 Then
    nxx = 65536 + xx
  Else
    nxx = xx
  End If
  xl = (nxx And 255)
  xh = (nxx - xl) / 256
  logfile.Write ("0x" + Hex$(xl) + "," + "0x" + Hex$(xh))
  
  If EOF(1) Then Exit Do
  'sinon ,
  logfile.Write (",")
   
  nw = nw + 1
  If nw > 7 Then
    logfile.Write (Chr$(13))
    nw = 0
  End If
  
Loop
Close #1

logfile.Write ("};")
logfile.Close
 
MsgBox f$ + " codé en " + f2$
Unload Me

End Sub
