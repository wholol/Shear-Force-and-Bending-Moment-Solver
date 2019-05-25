#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <iomanip>      // std::setprecision
using namespace std;

class beamProperties {

protected:
	double beamlength; int PointLoadNumber; double PointLoadMagnitude; double PointLoadLocation; double SupportLocation;
	vector <double> PointLoadLocationVect; vector <double> PointLoadMagnitudeVect;		//vectors for point load magnitude and location.
	double SupportLocationArray[2];

	int UDLnumber; double UDLmagnitude; double UDLbegin; double UDLend;
	vector <double> UDLlocation_beginVect; vector <double> UDLlocation_endVect; vector <double> UDLmagnitudeVect;

public:

	void initbeam() {
		cout << "Welcome to the shear force and bending moment solver. Enter beam length to get started." << endl;
		cin >> beamlength;
	}

	void PointLoadSetup() {
		cout << "Enter number of point loads that you have." << endl;
		cin >> PointLoadNumber;
		cout << "   " << endl;
		cout << "Enter Point Load magnitude, followed by its location." << endl;
		for (int i = 0; i < PointLoadNumber; ++i) {
			cout << "   " << endl;
			cout << "enter point load magnitude." << endl;
			cin >> PointLoadMagnitude;
			PointLoadMagnitudeVect.push_back(PointLoadMagnitude);
			cout << "   " << endl;
			cout << "Enter Point Load location" << endl;
			cin >> PointLoadLocation;
			PointLoadLocationVect.push_back(PointLoadLocation);
		}
	}

	void UDLSetup() {		//uniform distributed loading setup.
		cout << " enter number of distributed loads." << endl;
		cin >> UDLnumber;
		for (int i = 0; i < UDLnumber; i++) {
			cout << "Enter distributed loading w value, followed by its beginninglocation, lastly the end location. (start from left)." << endl;
			cin >> UDLmagnitude;
			UDLmagnitudeVect.push_back(UDLmagnitude);
			cout << " " << endl;
			cout << "enter beginning of location (from left)." << endl;
			cin >> UDLbegin;
			UDLlocation_beginVect.push_back(UDLbegin);
			cout << " " << endl;
			cout << " enter ed of location (from right)" << endl;
			cin >> UDLend;
			UDLlocation_endVect.push_back(UDLend);

		}
	}

	void Supportsetup() {
		for (int i = 0; i < 2; i++) {
			cout << "Enter support location number  " << i + 1 << endl;
			cin >> SupportLocation;
			SupportLocationArray[i] = SupportLocation;
		}

		if (SupportLocationArray[0] > SupportLocationArray[1]) {
			swap(SupportLocationArray[0], SupportLocationArray[1]);			//let SupportLocationArray[0] be the smallest value.
		}
	}
};


class ComputeSupports :public beamProperties {
protected:
	vector <double> LeftSupports; vector <double> RightSupports;		//these vectors are for the superposition vectors for right and left supports.
	

public:

	void PointLoadSupports() {			//determine the support value for each instance.
		for (int i = 0; i < PointLoadNumber; i++) {
			RightSupports.push_back((-(PointLoadLocationVect[i] - SupportLocationArray[0])*PointLoadMagnitudeVect[i])/(SupportLocationArray[1] - SupportLocationArray[0]));	//calcualte each instance of the superposition left support
			LeftSupports.push_back(((PointLoadLocationVect[i] - SupportLocationArray[1])*PointLoadMagnitudeVect[i])/(SupportLocationArray[1] - SupportLocationArray[0]));	//calculates each instance of the superposition right support
		}

		for (int i = 0; i < LeftSupports.size(); i++) {
			cout << LeftSupports[i] << " left is left, rght is " << RightSupports[i] << endl;
		}
	}

	void UDLSupports() {			//determine the support values for UDL.
		for (int i = PointLoadNumber; i < (PointLoadNumber + UDLnumber); i++) {	//this for loop stores the UDL support values into the Left and Right Support vectors
			for (int j = 0; j < UDLnumber; j++) {								//this loop performs the computation for the supports.
				LeftSupports.push_back(((UDLlocation_endVect[j] - UDLlocation_beginVect[j])*UDLmagnitudeVect[j])*(0.5*(UDLlocation_endVect[j] + UDLlocation_beginVect[j]) - SupportLocationArray[0]));
				RightSupports.push_back(((UDLlocation_endVect[j] - UDLlocation_beginVect[j])*UDLmagnitudeVect[j])*(0.5*(UDLlocation_endVect[j] + UDLlocation_beginVect[j]) - SupportLocationArray[1]));
			}
		}
	}

};

class ShearForces :public ComputeSupports{
protected:
	vector <double> PointForceVal;			//stores all point force values, including support reactions.
	vector <double> PointForceLocal;		//stores all point force locations, including support values.
	vector <pair <double, double>> PointLoadPair;					//pairs the pint load magnitude and location.
	vector <double> PointShearVect;

	vector <double> ShearLocationSuperposition;		// this location vector stores the location for shear for each superposition
	vector <double> PointShearSuperposition;		// this value stores the values for the point load shears for each superposition.
	vector <double> ShearLocationFinal;				//final plots after superpostion
	vector <double> PointShearFinal;				//final shear values.
	vector <double> removeShear;					//remove redundant shear laods by the point load
	int element = 0;

public:
	void PointLoadShearSetup() {			//this function computes each instance of the point load shear
		for (int i = 0; i < PointLoadNumber; i++){
			PointForceVal.push_back(RightSupports[i]);					//pushes back the right support value
			PointForceVal.push_back(LeftSupports[i]);					//pushes back the left support value
			PointForceVal.push_back(PointLoadMagnitudeVect[i]);			//pushes back the point load magnitude vector
			PointForceLocal.push_back(SupportLocationArray[1]);
			PointForceLocal.push_back(SupportLocationArray[0]);
			PointForceLocal.push_back(PointLoadLocationVect[i]);	
		}

		for (int i = 0; i < PointForceLocal.size() ; i++) {								//sorting the store_location vetor and store_force vector
			PointLoadPair.push_back(make_pair(PointForceLocal[i], PointForceVal[i]));	//sorts location vector.
		}

		for (float i = 0; i <= beamlength; i += 0.2) {		//location vector for the final shear
			ShearLocationFinal.push_back(i);
		}

		/*for (int i = 0; i < PointLoadPair.size(); i++) {
			cout << PointLoadPair[i].first << " <- local force-> " << PointLoadPair[i].second << endl;
		}*/

		for (int i = 0; i < PointLoadPair.size(); i += 3) {								// sorts the location of point forces according.
			if (PointLoadPair[i].first > PointLoadPair[i + 1].first) {
				swap(PointLoadPair[i].first, PointLoadPair[i + 1].first);
				swap(PointLoadPair[i].second, PointLoadPair[i + 1].second);
			}

			if (PointLoadPair[i + 1].first > PointLoadPair[i + 2].first) {
				swap(PointLoadPair[i + 2].first, PointLoadPair[i + 1].first);
				swap(PointLoadPair[i + 2].second, PointLoadPair[i + 1].second);
			}

			if (PointLoadPair[i].first > PointLoadPair[i + 1].first) {
				swap(PointLoadPair[i].first, PointLoadPair[i + 1].first);
				swap(PointLoadPair[i].second, PointLoadPair[i + 1].second);
			}
		}

		for (int i = 0; i < PointLoadPair.size(); i++) {
			cout << "force number  " << i << endl;
			cout << PointLoadPair[i].first << " <- location of forces/supports || local force-> " <<  PointLoadPair[i].second << endl;
		}
	}

	void ComputePointShear() {//computes the shear forces for the point load forces.

		for (int i = 0; i < PointLoadPair.size(); i += 3) {
			PointShearVect.push_back(PointLoadPair[i].second);
			PointShearVect.push_back(PointLoadPair[i].second + PointLoadPair[i + 1].second);
		}

		for (int i = 0; i < PointShearVect.size(); i++) {			//this loop computes the local shear forces. take note that each superpostion will only have two shear forces (since each superposition will have two supports + one poit force only.
			cout << PointShearVect[i] << "<-- ShearForces " << PointLoadPair[i + 1].first << endl;	//critical points at which the forces change values.	
		}

		int shearsize = 0;			//initialize varialbe called shear size
		int increment = 0;		
		while (1) {			//this while loop produces the
			for (int h = 3; h < PointLoadPair.size(); h += 3) {	
				if (increment == h - 1) {		//since we dont want to compute from the last element of the first force to the beginning of the second force.
					increment = h;
				}
			}
			//cout << j << " " << shearsize << endl;

			for (int i = 0; i < PointLoadPair.size(); i+=3) {						//this loop adds zeros in the event where the smallest location is not zero (to balance the vector segments).
				if (increment == i && PointLoadPair[i].first != 0) {
					for (double j = 0; j <= (PointLoadPair[increment].first) * 10; j++) {
						ShearLocationSuperposition.push_back(j / 10);
						PointShearSuperposition.push_back(0);
					}
				}
			}

			for (double k = (PointLoadPair[increment].first) * 10; k <= (PointLoadPair[increment + 1].first) * 10; k++) {		//for each point load pair location.
				ShearLocationSuperposition.push_back(k / 10);											//push back the location distance, incremented by 0.2
				PointShearSuperposition.push_back(PointShearVect[shearsize]);							//push back the shear force value.
						
				for (int i = 1; i < PointLoadPair.size(); i += 3) {										//inserts extra shear values due at the transition region for plotting.
					if (k == PointLoadPair[i].first*10) {												//inserts extra shear values in transitino regino for plotting						
							ShearLocationSuperposition.push_back(k / 10);
							PointShearSuperposition.push_back(PointShearVect[shearsize]);		
					}
				}
			}


			for (int y = 2; y < PointLoadPair.size(); y += 3) {			//checks the biggest location to see if it is the beamlength or not. This loop is used to balanced arrays as well.
				if (PointLoadPair[y].first != beamlength && increment + 1 == y) {				//if the last location of each force instance is not the beamlengh itself
					for (double l = PointLoadPair[increment + 1].first*10; l <= beamlength*10; l++) {
						ShearLocationSuperposition.push_back(l / 10 + (0.2));
						PointShearSuperposition.push_back(0);		//push back zero
					}
				}
			}

			increment = increment + 1;
			shearsize += 1;
			if (shearsize >= PointShearVect.size()) {
				break;
			}	
		}		

		for (int i = 1; i < ShearLocationSuperposition.size() - 1; i++) {
			for (int j = 2; j < PointLoadPair.size(); j += 3) {
				for (int k = 0; k < PointLoadPair.size(); k += 3) {
					if ((PointShearSuperposition[i - 1] != PointShearSuperposition[i]) && (ShearLocationSuperposition[i] == ShearLocationSuperposition[i - 1])) {	//if the force value is different and the location is the same, provided that the location is not the first or lat location.
						cout << i << endl;
						cout << PointShearSuperposition[i - 1] << " " << PointShearSuperposition[i] << " " << ShearLocationSuperposition[i] << " " << ShearLocationSuperposition[i - 1] << endl;
						removeShear.push_back(i);
					}
				}
			}
		}

		for (int j = 0; j < removeShear.size(); j++) {		//thi loop is rsponsible fo removing assitional shear forces caused by the addtion of shear forces.
			PointShearSuperposition.erase(PointShearSuperposition.begin() + removeShear[j] - element);
			PointShearSuperposition.erase(PointShearSuperposition.begin() + removeShear[j] - 1 - element);
			ShearLocationSuperposition.erase(ShearLocationSuperposition.begin() + removeShear[j] - element);
			ShearLocationSuperposition.erase(ShearLocationSuperposition.begin() + removeShear[j] - 1 - element);
			element += 2;
		}
		
		for (int i = 0; i < ShearLocationSuperposition.size(); i++) {
			cout << ShearLocationSuperposition[i] << " <--- Location Plot || Shear value Superposition --> " << PointShearSuperposition[i] << endl;
		}

		for (int i = 0; i <= (ShearLocationSuperposition.size() / PointLoadNumber) - 1; i++) {	//pushes back the first force values into the final shear force array
			PointShearFinal.push_back(PointShearSuperposition[i]);
		}

		if (PointLoadNumber != 1) {					//if there is more than one point load, the superposition vector is split into parts and added to each index.
			for (int k = 1; k < PointLoadNumber; k++) {		//for each each instance of point load.
				for (int i = 0; i <= (ShearLocationSuperposition.size() / PointLoadNumber) - 1; i++) { //this loop sums all the point loads together.
					PointShearFinal[i] += PointShearSuperposition[k*(ShearLocationSuperposition.size() / PointLoadNumber) + i];	//sums the superposition forces together
				}
			}
		}
		
		for (int i = 0; i < PointShearFinal.size(); i++) {
			cout << ShearLocationSuperposition[i] << "<----LOCATION || SHEAR VALUE FINAL ---->" << PointShearFinal[i] <<  endl;
		}
	}
	

	void UDLShearSetup() {

	}

};

class BendingMoments :public ShearForces {
protected:

public:

	void ComputePointLoadMoments() {

	}

};

int main() {

	ShearForces beam;
	beam.initbeam();
	beam.PointLoadSetup();
	beam.Supportsetup();
	beam.PointLoadSupports();
	beam.PointLoadShearSetup();
	beam.ComputePointShear();
	system("pause");

}
