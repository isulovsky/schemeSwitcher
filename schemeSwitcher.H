/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Version:  v2012
     \\/     M anipulation  |
\*---------------------------------------------------------------------------*/

#ifndef schemeSwitcher_H
#define schemeSwitcher_H

#include "functionObject.H"
#include "dictionary.H"
#include "fvMesh.H"
#include "HashTable.H"

namespace Foam
{

class schemeSwitcher
:
    public functionObject
{
    // Private data

    //- Reference to object registry
    const objectRegistry& obr_;

    //- Time-scheme mapping (using word as key type)
    HashTable<dictionary, word> schemeSwitch_;

    //- Last switched time
    scalar lastSwitchTime_;

    //- Return mesh from objectRegistry
    const fvMesh& mesh() const
    {
        const fvMesh* meshPtr = obr_.findObject<fvMesh>("region0");
        if (!meshPtr)
        {
            meshPtr = obr_.findObject<fvMesh>(polyMesh::defaultRegion);
        }
        
        if (!meshPtr)
        {
            FatalError
                << "No mesh found" << exit(FatalError);
        }
        
        return *meshPtr;
    }

public:
    //- Runtime type information
    TypeName("schemeSwitcher");

    // Constructors

    //- Construct from Time and dictionary
    schemeSwitcher
    (
        const word& name,
        const Time& runTime,
        const dictionary& dict
    );

    //- No copy construct
    schemeSwitcher(const schemeSwitcher&) = delete;

    //- No copy assignment
    void operator=(const schemeSwitcher&) = delete;

    //- Destructor
    virtual ~schemeSwitcher() = default;

    // Member Functions

    //- Read the schemes data
    virtual bool read(const dictionary&);

    //- Execute, called at each time step
    virtual bool execute();

    //- Write, currently does nothing
    virtual bool write();
};

} // End namespace Foam

#endif