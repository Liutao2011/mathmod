/***************************************************************************
 *   Copyright (C) 2019 by Abderrahman Taha                                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor,Boston, MA 02110-1301 USA             *
 ***************************************************************************/

#include <map>
#include <vector>
#include <iostream>
#include <string.h>
#include <cmath>
#include "../objectparameters.h"
#include "ND/Matrix4D.h"
#include <qthread.h>
using std::string;

struct   ParStruct
{
    std::string fx;
    std::string fy;
    std::string fz;
    std::string fw;
    std::string cnd;
    std::string umin;
    std::string umax;
    std::string vmin;
    std::string vmax;
    std::string grid;
    int  index;
};

class ParWorkerThread : public QThread
{
    Q_OBJECT
public :
    uint MyIndex, WorkerThreadsNumber, Ugrid, Vgrid;
    FunctionParser * myParserX, * myParserY,* myParserZ, *myParserW, *Fct;
    double  *v_inf, *v_sup,*u_inf,*u_sup,*dif_v,*dif_u;
    double stepMorph, pace;
    int activeMorph, param4D;
    uint iStart, iFinish;

    bool StopCalculations, ParsersAllocated;
    unsigned int CurrentPar, CurrentIndex;
    int signalVal;

public :
    void ParCompute(uint, uint);
    void calcul_objet(uint component =0, uint idx=0);
    void AllocateParsersForWorkerThread(uint, uint);
    void DeleteWorkerParsers();
    void run() Q_DECL_OVERRIDE;
    ParWorkerThread();
    ~ParWorkerThread() override;
signals:
    void mySignal(int);
public:
    void emitMySignal();
};

class ParMasterThread : public ParWorkerThread
{
public :
    ErrorMessage stdError;
    FunctionParser *RgbtParser, *VRgbtParser, *GradientParser, *NoiseParser, *NoiseShapeParser;
    FunctionParser *myParserCND,
                   *myParserUmin,*myParserUmax,
                   *myParserVmin,*myParserVmax,
                   *Var, Cstparser;
    ParStruct *ParamStructs;
    bool*UsedFunct, *UsedFunct2, gridnotnull;
    uint *grid;
    uint Nb_paramfunctions, Nb_functs, Nb_constants;
    uint Nb_rgbts, Nb_vrgbts, Nb_newvariables;
    std::string  expression_X, expression_Y, expression_Z, expression_W, expression_CND, inf_u, sup_u, inf_v, sup_v,
        Varu, Const,  Funct, Rgbt, Grid;
    int Nb_Sliders, ParConditionRequired, expression_XSize, expression_YSize, expression_ZSize, expression_WSize, expression_CNDSize,
        inf_uSize, sup_uSize, inf_vSize, sup_vSize,
        VaruSize, ConstSize,  FunctSize, RgbtSize, GridSize;
    std::string *VarName, *Varus,
        *Consts, *ConstNames,
        *Functs, *FunctNames,
        *Rgbts, *RgbtNames,
        VRgbt, *VRgbts, *VRgbtNames,
        Gradient, Noise, NoiseShape, *SliderNames;
    double *ConstValues, *SliderValues;
    unsigned int NbPolygnNbVertex[2], nbBorderPts;
    double Lacunarity, Gain;
    int Octaves;
public :
    void InitMasterParsers();
    uint  HowManyParamSurface(std::string, int);
    uint  HowManyVariables(std::string, int);
    ErrorMessage parse_expression();
    void AllocateParsersForMasterThread();
    void AllocateParsersForThread();
    void DeleteMasterParsers();
    ParMasterThread();
    ~ParMasterThread();
};

/** The representation of a 3D model */
class Par3D   : public QThread
{
    Q_OBJECT
public:
    ObjectProperties *LocalScene;
    ParMasterThread *masterthread;
    ParWorkerThread *workerthreads;
    //unsigned int *IndexPolyTab;
    //unsigned int *IndexPolyTabMin;
    float *Border;
    bool *WichPointVerifyCond, StopCalculations;
    int *TypeIsoSurfaceTriangleListeCND;
    int VerifCND[1000000];
    uint NbVertex, WorkerThreadsNumber;
    uint Ugrid, Vgrid;
    uint CutV, CutU;
    float MINX,MINY,MINZ,MINW,
           MAXX,MAXY,MAXZ,MAXW,
           DIFX,DIFY,DIFZ,DIFW,
           DIFMAXIMUM;
    Matrix4D mat4D, mat_rotation4D, mat_rotation_save4D,
             mat_homothetie4D, mat_translation4D, mat_inversetranslation4D;
    double tetaxy, tetaxz, tetayz, tetaxw, tetayw, tetazw;

    int tetaxy_ok, tetaxz_ok, tetayz_ok, tetaxw_ok, tetayw_ok, tetazw_ok, param4D;
    int largeur_fenetre,hauteur_fenetre;
    ScriptErrorType messageerror;
    QString message;
public:
    Par3D(uint,
          uint nbThreads=16,
          uint nbGrid=50);
    ~Par3D()  override;
    void rotation4();
    void calcul_points4(uint idx=0);
    void Anim_Rot4D (uint idx=0);
    void project_4D_to_3D(uint idx=0);
    void Invert_boite_englobante4D(uint idx=0);
    void boite_englobante4D(uint index=0);
    void initialiser_parametres(uint, uint);
    void initialiser_LineColumn(uint, uint);
    void calcul_Norm(uint i=0);
    void make_PolyIndexTri(uint i=0, uint p=0, uint  IsoPos=0);
    void make_PolyIndexMin(uint i=0, uint p=0, uint  IsoPos=0);
    uint CNDCalculation(uint &, struct ComponentInfos *);
    void CalculateColorsPoints(struct ComponentInfos *);
    void ParamBuild(float *, float *,unsigned int *, unsigned int *,
                    unsigned int *, uint  IsoPos=0,
                    ComponentInfos *components = nullptr,
                    int *TriangleListeCND = nullptr,
                    bool *typeCND = nullptr,
                    unsigned int *IndexPolyTabMinPt = nullptr,
                    unsigned  int *NbPolyMinPt = nullptr);
    void BuildPar();
    void UpdateThredsNumber(uint);
    void stopcalculations(bool);
    void WorkerThreadCopy(ParWorkerThread *);
    void MasterThreadCopy(ParMasterThread *);
    ErrorMessage ThreadParsersCopy();
    ErrorMessage  parse_expression2();
    ErrorMessage  ParMorph();
    void copycomponent(struct ComponentInfos*, struct ComponentInfos*);
    void run() Q_DECL_OVERRIDE;
public :
    void emitErrorSignal();
    void emitUpdateMessageSignal();
signals:
    void ErrorSignal(int);
    void UpdateMessageSignal(QString);
};
