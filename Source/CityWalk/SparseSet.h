//#include <concepts>

#include "CoreMinimal.h"

//template <typename Key>
//
//template <class DenseClass>
//concept Component = requires(DenseClass &D) 
//{
//    {
//        D.GetKey()
//    } -> std::same_as<Key>;
//};

template <typename Key, typename Index, class DenseClass>
class TSparseSet
{

private:

    TArray<DenseClass> Dense;
    TMap<Key, Index> Sparse;

public:

    FORCEINLINE TSparseSet(const int32 & ReservedSize)
    {
        Dense.Reserve(ReservedSize);
        Sparse.Reserve(ReservedSize);
    }

    FORCEINLINE TSparseSet()
    {
    
        const int32 BasicReservedSize = 32;

        Dense.Reserve(BasicReservedSize);
        Sparse.Reserve(BasicReservedSize); 
    
    }

    /**
    * Adds Ony Unique Elements to the Sparse 
    * @returns Index of the Element Or Size - 1 if element is new 
    */
    Index TryAdd(const Key& KeyValue, const DenseClass& Object) 
    {

        Index* I = Sparse.Find(KeyValue);

        if (I)
        {
            return *I;
        }

        return Add(KeyValue, Object);

    }

    /**
    * Adds or updates a given Key + Object Pair
    * If Object is Already in the set: Update Key + Idx in Sparse and add Object to Dense 
    */
    FORCEINLINE Index Add(const Key& KeyValue, const DenseClass & Object)
    {
        const Index Idx = Dense.Add(Object);

        Sparse.Add(KeyValue, Idx);

        return Dense.Num() - 1;

    }

    DenseClass* GetByKey(const Key& Key)
    {

        Index* Idx = Sparse.Find(Key);

        if (!Idx) { return nullptr; }

        if (!Dense.IsValidIndex(*Idx)) { return nullptr; }

        return &Dense[*Idx];

    }

    FORCEINLINE DenseClass * GetByIndex(const Index &Idx)
    {
        if (!Dense.IsValidIndex(Idx)) { return nullptr; }
    
        return &Dense[Idx];

    }

    bool Remove(const Index& Idx)
    {

        if (!Dense.IsValidIndex(Idx)) { return false; }

        DenseClass* D = &Dense[Idx];
        if (!D) { return false; }

        Key OldKey = D->GetKey();

        int32 DenseSize = Dense.Num();

        Dense.RemoveAtSwap(Idx);
        Sparse.Remove(OldKey);

        if (Idx != DenseSize - 1)
        {
            DenseClass* D = &Dense.Last();
            Sparse[D->GetKey()] = Idx;
        }

        return true;

    }

    bool Remove(const DenseClass* D)
    {

        if (!D) { return false; }

        Key OldKey = D->GetKey();
        Index* Idx = Sparse.Find(OldKey);
        if (!Idx) { return false; }

        return Remove(*Idx);

    }

    FORCEINLINE int32 GetNum() const { return Dense.Num(); };

    /** Returns Reserved Size of Both Arrays **/
    FORCEINLINE int32 GetSize() const { return Dense.Max(); };

};