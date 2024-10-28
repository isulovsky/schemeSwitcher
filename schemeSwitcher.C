/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Version:  v2012
     \\/     M anipulation  |
\*---------------------------------------------------------------------------*/

#include "schemeSwitcher.H"
#include "Time.H"
#include "polyMesh.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(schemeSwitcher, 0);
    addToRunTimeSelectionTable(functionObject, schemeSwitcher, dictionary);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::schemeSwitcher::schemeSwitcher
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    functionObject(name),
    obr_(runTime.db()),
    schemeSwitch_(),
    lastSwitchTime_(-1.0)
{
    read(dict);
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::schemeSwitcher::read(const dictionary& dict)
{
    const dictionary& switchesDict = dict.subDict("schemeSwitch");
    
    // Clear existing schemes
    schemeSwitch_.clear();
    
    // Read all time entries
    forAllConstIter(dictionary, switchesDict, iter)
    {
        const word& timeKey = iter().keyword();
        schemeSwitch_.insert(timeKey, dictionary(iter().dict()));
    }
    
    return true;
}

bool Foam::schemeSwitcher::execute()
{
    const Time& runTime = obr_.time();
    const scalar currentTime = runTime.value();
    
    const fvMesh& mesh = this->mesh();

    // Check if we need to switch schemes at this time
    HashTable<dictionary, word>::const_iterator iter = schemeSwitch_.begin();
    for (; iter != schemeSwitch_.end(); ++iter)
    {
        const scalar switchTime = std::stod(iter.key().c_str());
        
        if (currentTime >= switchTime && switchTime > lastSwitchTime_)
        {
            IOdictionary& fvSchemesDict = const_cast<IOdictionary&>
            (
                mesh.lookupObject<IOdictionary>("fvSchemes")
            );
            
            // Apply the new schemes
            const dictionary& schemesDict = iter();
            
            forAllConstIter(dictionary, schemesDict, schemeIter)
            {
                const word& subDictName = schemeIter().keyword();
                const dictionary& subDict = schemesDict.subDict(subDictName);
                
                if (fvSchemesDict.found(subDictName))
                {
                    dictionary& targetSubDict = fvSchemesDict.subDict(subDictName);
                    
                    forAllConstIter(dictionary, subDict, entryIter)
                    {
                        const word& entryName = entryIter().keyword();
                        const word newScheme(entryIter().stream());
                        
                        targetSubDict.set(entryName, newScheme);
                        Info<< "Time = " << currentTime 
                            << ": Switched " << subDictName << "::" 
                            << entryName << " to " << newScheme << endl;
                    }
                }
            }
            
            lastSwitchTime_ = switchTime;
        }
    }
    
    return true;
}

bool Foam::schemeSwitcher::write()
{
    return true;
}

// ************************************************************************* //