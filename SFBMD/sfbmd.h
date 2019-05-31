#pragma once
#include <iostream>
#include <vector>

using namespace std;


class beamProperties {

protected:
	double beamlength; int PointLoadNumber; double PointLoadMagnitude; double PointLoadLocation; double SupportLocation;
	vector <double> PointLoadLocationVect; vector <double> PointLoadMagnitudeVect;		//vectors for point load magnitude and location.
	double SupportLocationArray[2];

	int UDLnumber; double UDLmagnitude; double UDLbegin; double UDLend;
	vector <double> UDLlocation_beginVect; vector <double> UDLlocation_endVect; vector <double> UDLmagnitudeVect;
	
public:
	void initbeam();
	void PointLoadSetup();
	void UDLSetup();
	void SupportSetup();
	int getPointLoadNumber() const;
	int getUDLNumber() const;
};

class ComputeSupports : public beamProperties {
protected:
	vector <double> LeftSupports; vector <double> RightSupports;		//these vectors are for the superposition vectors for right and left supports.
	vector <double> UDLLeftSupports; vector <double> UDLRightSupports;

public:
	void PointLoadSupports();
	void UDLSupports();
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


	vector <double> UDLForceVal;	//includes all force values, including
	vector <double> UDLForceLocal;
	vector <double> UDLSuperpositionLocation;
	vector <double> UDLSuperpositionShear;
	vector <double> UDLShearFinal;
	vector <double> UDLLocationFinal;

public:
	void PointLoadShearSetup();
	void ComputePointShear();
	void computeUDLShear();
	void ParseShearData();			//parses data to excel
};

class BendingMoments :public ShearForces {
protected:
	vector <double> PointLoadSuperPositionMoments;
	vector <double> PointMomentsFinal;

	vector <double> UDLSuperPositionMoments;
	vector <double> UDLMomentsFinal;

public:
	void ComputePointLoadMoments();
	void computeUDLMoments();
	void ParseMomentsData();
};
