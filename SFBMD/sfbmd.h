#pragma once
#include <iostream>
#include <vector>

using namespace std;


class beamProperties {

protected:
	double beamlength; int PointLoadNumber; double PointLoadMagnitude; double PointLoadLocation;		//variables for user input.
	vector <double> PointLoadLocationVect; vector <double> PointLoadMagnitudeVect;		//vectors for point load magnitude and location.
	double SupportLocationArray[2];														//stores the support locations.

	int UDLnumber; double UDLmagnitude; double UDLbegin; double UDLend;				//variables for user input for UDL.
	vector <double> UDLlocation_beginVect; vector <double> UDLlocation_endVect; vector <double> UDLmagnitudeVect;		//vectors to store the beginning, end and magnitude for UDL.
	
	int TDLnumber; double TDLmagnitude; double TDLbegin; double TDLend; char TDLgradient;										//variables for user input for TDL.
	vector <double> TDLlocation_beginVect; vector <double> TDLlocation_endVect; vector <double> TDLmagnitudeVect; vector <char> TDLgradientVect;				//vectors to store beginning, end and magitude for TDL.
	
public:
	void initbeam();		//initialize the beamlength.
	void PointLoadSetup();	//member function to set up point loads (user inputs get asked)
	void UDLSetup();		//member function to set up UDL loads(user inputs get asked)
	void TDLSetup();		//member function to set up triangular loads(user inputs get asked)
	void SupportSetup();	//member fuction to setup support
	int getPointLoadNumber() const;	// gets the pointloadnumber variable.
	int getUDLNumber() const;		//gets the UDL number variable.
	int getTDLnumber() const;		//gets the TDL number variable
};

class ComputeSupports : public beamProperties {
protected:
	vector <double> LeftSupports; vector <double> RightSupports;		//these vectors are for the superposition vectors for right and left supports for point loads.
	vector <double> UDLLeftSupports; vector <double> UDLRightSupports;	//these are for UDL supports.
	vector <double> TDLLeftSupports; vector <double> TDLRightSupports;	//these are for TDL supports.

public:
	void PointLoadSupports();	//this function computes the point load supports.
	void UDLSupports();			//this function computes the UDL supports.
	void TDLSupports();			//this function computes te TDL supports.
};

class ShearForces :public ComputeSupports {
protected:
	vector <double> PointForceVal;			//stores all point force values, including support reactions.
	vector <double> PointForceLocal;		//stores all point force locations, including support values.
	vector <pair <double, double>> PointLoadPair;					//pairs the pint load magnitude and location.
	vector <pair <double, double>> PointShearFinalPair;					//pairs the final shear force value and location for point load.
	vector <double> PointShearVect;				//stores the shear forces.

	vector <double> ShearLocationSuperposition;		// this location vector stores the location for shear for each superposition
	vector <double> PointShearSuperposition;		// this value stores the values for the point load shears for each superposition.
	vector <double> ShearLocationFinal;				//final plots after superpostion
	vector <double> PointShearFinal;				//final shear values.
	vector <double> removeShear;					//remove redundant shear laods by the point load
	int element = 0;
	int storeLastLocation;							//stores the last element of the shear lcoation before adding zeros.

	vector <double> UDLSuperpositionLocation;		//superpostion location for UDL.
	vector <double> UDLSuperpositionShear;			//superposition shear force for UDL.
	vector <double> UDLShearFinal;					//final shear values for UDL.
	vector <double> UDLLocationFinal;				//final location for UDL.


	vector <double> TDLSuperpositionLocation;		//superpostion location for UDL.
	vector <double> TDLSuperpositionShear;			//superposition shear force for UDL.
	vector <double> TDLShearFinal;					//final shear values for UDL.
	vector <double> TDLLocationFinal;				//final location for UDL.

public:
	void PointLoadShearSetup();						//sets up ponit laod shear force values.
	void ComputePointShear();						//computes point load shear force
	void computeUDLShear();							//computes UDL shear
	void computeTDLShear();							//computes TDL shear
	void ParseShearData();			//parses data to excel
};

class BendingMoments :public ShearForces {
protected:
	vector <double> PointLoadSuperPositionMoments;
	vector <double> PointMomentsFinal;

	vector <double> UDLSuperPositionMoments;
	vector <double> UDLMomentsFinal;

	vector <double> TDLSuperPositionMoments;
	vector <double> TDLMomentsFinal;

public:
	void ComputePointLoadMoments();
	void computeUDLMoments();
	void computeTDLMoments();
	void ParseMomentsData();
};
