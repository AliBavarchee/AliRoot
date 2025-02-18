#ifndef ALIITSV11_H
#define ALIITSV11_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//========================================================================
//
//                 Inner Traking System geometry v11
//
//  Based on ROOT geometrical modeler
//
// B. Nilsen, L. Gaudichet, M. Sitta
//
//========================================================================


// $Id: 

// $Log$
// Revision 1.1  2011/06/10 14:48:24  masera
// First version from v11Hybrid to v11 (M. Sitta)
//
 
#include "AliITS.h"
#include "AliITSInitGeometry.h"

class  AliITSv11GeometrySPD;
class  AliITSv11GeometrySDD;
class  AliITSv11GeometrySSD;
class  AliITSv11GeometrySupport;
class  TGeoVolume;
class  TGeoVolumeAssembly;

class AliITSv11 : public AliITS {

 public:
enum kITSDensRegion {kSPDSiChip, kSPDSiSens, kSPDAlBus, kSPDCoolPipes,
                     kSDDSiAll, kTOTAL};

    AliITSv11();
    AliITSv11(const char *title);
    AliITSv11(const char *name, const char *title);
    virtual       ~AliITSv11() ;

    virtual void   AddAlignableVolumes() const;
    virtual void   CreateGeometry();
    virtual void   CreateMaterials();

    virtual AliITSv11GeometrySPD*     GetSPDGeometry(){return fSPDgeom;}
    virtual AliITSv11GeometrySDD*     GetSDDGeometry(){return fSDDgeom;}
    virtual AliITSv11GeometrySSD*     GetSSDGeometry(){return fSSDgeom;}
    virtual AliITSv11GeometrySupport* GetSupGeometry(){return fSupgeom;}

    virtual Int_t  IsVersion() const {// returns the ITS version number 
                                      return 11;} 
    virtual void   Init(); 
    virtual void   SetDefaults();
    virtual void   StepManager();
    virtual void SetDensityServicesByThickness(){// uses services density
	// calculation based on the thickness of the services.
	fByThick = kTRUE;}
    virtual void SetDensityServicesByMass(){// uses services density
	// calculation based on the Mass of the services.
	fByThick = kFALSE;}

    virtual void SetDensityFactor(const kITSDensRegion reg, const Double_t fact)
        { fDensityFactors[reg] = fact; };

 protected:
    void SetT2Lmatrix(Int_t uid, Double_t yShift,
		      Bool_t yFlip, Bool_t yRot180=kFALSE) const; // Set T2L matrix in TGeoPNEntries

 private:
    AliITSv11(const AliITSv11 &source); // copy constructor
    AliITSv11& operator=(const AliITSv11 &source); // assignment operator

    Bool_t fByThick;          // Flag to use services materials by thickness
                              // ture, or mass false.
    Int_t  fIDMother;         //! ITS Mother Volume id.

    Double_t fDensityFactors[kTOTAL];

    AliITSInitGeometry fInitGeom;   //! Get access to decoding and AliITSgeom init functions
    AliITSv11GeometrySPD     *fSPDgeom; //! SPD Geometry
    AliITSv11GeometrySDD     *fSDDgeom; //! SDD Geometry
    AliITSv11GeometrySSD     *fSSDgeom; //! SSD Geometry
    AliITSv11GeometrySupport *fSupgeom; //! Support Geometry

    ClassDef(AliITSv11,4)  // ITS version 11 
};
 
#endif
