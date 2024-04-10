'''
Camilla Lucero
12/5/2023
PJ01
 
This Program simulates caches from a block set of 8 bytes to 256 bytes, with associativty from 1 to 8192
--------------------------------------------------------------------------------------------------------
*I changed languages like twice, so there is some java looking comments in here!*
**Olivia Bach and I did a lot of code sharing between eachother, so our code may look similar in some
  parts as we worked together to figure out functionallity**

STRONGLY INFLUENCED BY THIS RESOURCE:
https://personal.ntu.edu.sg/smitha/ParaCache/Paracache/dmc.html
'''
import math

#Constants
MIN_BLOCK_SIZE= 8
MAX_BLOCKS_SIZE = 256
LRU = 0
FIFO = 1
WRITE_THROUGH = 0
WRITE_BACK = 1
WRITE_POLICY = WRITE_BACK #Change this based on preference

CACHE_SIZE = 65536 #64*1024

#Defines a block of data in a set of caches
class DataBlock:

    '''
    int tag; //the "identifier" for this bit of data
    int data; //the data within a data block
    boolean isValid; //The Valid bitIndicates whether this data is valid
    boolean isEmpty; //Indicates whether this data block is empty
    '''
    #Constructor
    def __init__(self, tag):

        #This bit of data will be given a tag when intialized
        #It will not the false and will always be empty on intialization
        self.tag = tag
        #self.data = None | We aren't doing data manipulation, so we don't need this
        self.isValid = False
        self.isEmpty = True
    
 
#defines a set of blocks in a cache
class CacheSet:
    '''
    DataBlock data[]; // An list of data that a set will hold
    '''

    #constructor
    def __init__(self,numBlocks):
        #In our set, create a certain number of blocks based on our numblocks calculation
        self.data = {} 
        for i in range(numBlocks):

            tempBlock = DataBlock(i)
            self.data[i] = tempBlock


#defines an entire cache
#direct-mapped: 1 set, multiple data blocks
#set associative: multiple sets, multiple data blocks
#full associative: multiple sets, 1 data block
class Cache: 
    '''
    int cacheSize;//How big this cache is allowed to be
    int blockSize;//How big blocks are allowed to be
    int associativity; // The number of bits of associativity of this cache
    int numSets;//The number of sets that this cache has
    CacheSet sets[]; // the sets that will actually contain data
    
    #Variables not directly related to the cache
    int hits;//How many hits has been counted when trying to read data
    int misses;// how many misses has been counted when trying to read data
    int reads;// how many times the cache has been read
    int writes;// how many times the cache has been written to
    int time; // how much time the cache took to run the 1000000 addresses
    '''
    
    #constructor
    def __init__(self, associativity, cacheSize, blockSize, replacementPolicy):
        self.cacheSize = cacheSize #Can be changed at the top of the code!
        self.blockSize = blockSize 
        self.associativity = associativity
        self.numSets = cacheSize // (self.blockSize * self.associativity)
        self.numBlocks = self.associativity


        #Create a bunch of sets in a dictionary
        self.sets = {}
        for i in range(self.numSets):

            tempSet = CacheSet(self.numBlocks)
            self.sets[i] = tempSet
    
        self.replacementPolicy = replacementPolicy
        #Meta data for Replacement Policy functionality
        self.metadata = [[] for _ in range(self.numSets)] #used for storing LRU & FIFO
        
       #Stuff not directly related to cache creation 
        self.hits = 0
        self.miss = 0
        self.reads = 0
        self.writes = 0
        self.accessTime = 0 

    def calculateAccessTime (self,cacheStatus):
        #All accesses, miss or not require an increase in access time
        #2ns for a direct mapped + 0.5 * log2(associativity) for every way 
        self.accessTime += (2 + (0.5 * math.log2(self.associativity))) #2ns + 0.5ns * log2(associativity)

        #If cache status is a hit, then we add 2ns
        if cacheStatus == "HIT":
            self.accessTime += 2
        
        elif cacheStatus == "MISS":
            if self.replacementPolicy == FIFO:
                self.accessTime += 1 #for a miss on FIFO, we add one more ns
            
            elif self.replacementPolicy == LRU:
                self.accessTime += 3 #for a miss on LRU, then add 3 more ns
            
            #Fixed +24 ns for misses
            self.accessTime += 24


    #Gets a block that has no data in it based on set
    def getFreeBlock(self, setNumber):
        blockSet = self.metadata[setNumber]
        
        #Checks for a block of data in the range of associativity. If that block is empty, it is returned
        for dataBlock in range(self.associativity):
            if self.sets[setNumber].data[dataBlock].isEmpty:
                return dataBlock

        #return the block set if there is no available space.
        if blockSet:
            return blockSet.pop(0)

        return None
    
    #gets the index of a datablock in a set of a specific tag
    def indexOf(self, tag, setNumber):
        #for a datablock in our associativity
        for dataBlock in range(self.associativity):
            #if our tag is not null, and our tag matches our given tag, then return data block
            if self.sets[setNumber].data[dataBlock].tag is not None and self.sets[setNumber].data[dataBlock].tag == tag:
                return dataBlock

        #if no match is found, then return -1
        return -1

    #MetaData, what we use for keeping track of LRU and FIFO
    def updateMetaData(self, setNumber, index):
        blockSet = self.metadata[setNumber]

        if not blockSet:
            blockSet.append(index)
        else:
            #this is where I specify LRU, otherwise, everything here works for FIFO
            #for FIFO, the order in block_set represents the order of access, 
            #while for LRU, it reflects the most recently used block at the end
            if self.replacementPolicy == LRU:
                target_index = next((i for i, x in enumerate(blockSet) if x == index), None)
                if target_index is not None:
                    blockSet.pop(target_index)

            blockSet.append(index)
    
    #Used when a cache needs to be read for information, either through accessing or for checking if a data block exists
    def read(self, tag, setNumber):

        #get index of data we want
        index = self.indexOf(tag, setNumber)

        #if that index is not -1, then that does exist in the cache, that's a hit
        if index != -1:
            #increaase hit
            self.hits += 1
            self.calculateAccessTime("HIT")

            #And update meta data for our cache 
            self.updateMetaData(setNumber, index)
        '''
        Because of how this is set up, in simulate cache, all reads will be hits and all misses will be writes
        #Else, then data does not exist, and that's a miss
        else:
            #increase miss
            #self.miss += 1

            #And intialize
            index = self.getFreeBlock(setNumber)
            block = self.sets[setNumber].data[index]

            block.tag = tag
            block.isEmpty = False
            self.reads += 1

            if WRITE_POLICY == WRITE_BACK:
                if block.isValid:
                    self.writes += 1

                block.isValid = False

            self.updateMetaData(setNumber, index)
        '''
    
    #Writes to a cache
    def write(self, tag, setNumber):
        #Find index of a data block
        index = self.indexOf(tag, setNumber)
        '''
        Because of how this is set up, in simulate cache, all reads will be hits and all misses will be writes
        if index != -1:
            #self.hits += 1
            block = self.sets[setNumber].data[index]

            block.tag = tag
            block.isEmpty = False

            if WRITE_POLICY == WRITE_THROUGH:
                self.writes += 1
            elif WRITE_POLICY == WRITE_BACK:
                block.isValid = True

            self.updateMetaData(setNumber, index)
        '''
        #if that data block is a -1, then that's a miss
        if index == -1:
            #update counter and add to our access time
            self.miss += 1
            self.calculateAccessTime("MISS")

            #Get index of a free block & initalize
            index = self.getFreeBlock(setNumber)
            block = self.sets[setNumber].data[index]
            block.tag = tag
            block.isEmpty = False

            #This counts as a read, so we iterate read counter also
            self.reads += 1

            #Indicate our read policy and update our write counter
            if WRITE_POLICY == WRITE_THROUGH:
                self.writes += 1
            elif WRITE_POLICY == WRITE_BACK:
                if block.isValid:
                    self.writes += 1
                self.sets[setNumber].data[index].isValid = True

            #Also update our metadata 
            self.updateMetaData(setNumber, index)
    
#Simulates a cache
def simulateCache(cache,blockSize, sequence):
    #for an address in a given list
    for address in sequence:
    
        # Calculate our tag and what set the address should be in should be in
        tag = address // blockSize
        try:
            setNumber = tag % cache.numSets
        
        except ZeroDivisionError:
            print("Divide by Zero Error, Skipping...")
            return -1

        #check if the data is in the cache (read operation).
        if cache.indexOf(tag, setNumber) != -1:
            cache.read(tag, setNumber)
        else:
            #cache miss - meaning data needs to be brought into the cache (write operation).
            cache.write(tag, setNumber)
#------------------------------------------------------------------------------------
#"Main"
# Load sequences from files
with open("random_access.txt", "r") as file:
    random_sequence = [int(line.strip()) for line in file]

with open("nonrandom_access.txt", "r") as file:
    nonrandom_sequence = [int(line.strip()) for line in file]

#calculate how big our blocks need to be
blockSize = [2**i for i in range(100) if 2**i >= MIN_BLOCK_SIZE and 2**i <= MAX_BLOCKS_SIZE ]

#max associativity. 
#I didn't know how to not hardcode this without a numBlock from a cache, but this does it for our purposes
maxAssociativity = [2**i for i in range(14)]

#iterate over possible cache configurations
#For our associativty in our precalculated list
for associativity in maxAssociativity:
    #for each size from min to max
    for size in blockSize:

        #header for each associativity & block size
        print(f"Associativity = {associativity}, BlockSize = {size}")
        print("-----------------------------------------------------------------------")
        #for each replacement policy
        for replacementPolicy in [LRU, FIFO]:
            
            #cache for random sequence
            randomCache = Cache(associativity, CACHE_SIZE, size, replacementPolicy)
            if randomCache == -1:
                break

            simulateCache(randomCache, size, random_sequence)
            print("Number of Sets = "  + str(randomCache.numSets))
            print("Number of Blocks = " + str(randomCache.numBlocks))

            #results for each configuration
            print(f"Random Config: Assoc. = {associativity}, Policy = {'LRU' if replacementPolicy == LRU else 'FIFO'}")
            print(f"Number of Reads = {randomCache.reads}, Number of Writes = {randomCache.writes}, Number of Misses = {randomCache.miss}, Number of Hits = {randomCache.hits}")
            print(f"Access Time = {randomCache.accessTime} nanoseconds\n") 
       
            #cache for nonrandom sequence
            nonrandomCache = Cache(associativity, CACHE_SIZE, size, replacementPolicy)
            simulateCache(nonrandomCache, size, nonrandom_sequence)
            print("Number of Sets = "  + str(nonrandomCache.numSets))
            print("Number of Blocks = " + str(nonrandomCache.numBlocks))

            #results for each configuration
            print(f"NonRandom Config: Assoc. = {associativity}, Policy = {'LRU' if replacementPolicy == LRU else 'FIFO'}")
            print(f"Number of Reads = {nonrandomCache.reads}, Number of Writes = {nonrandomCache.writes}, Number of Misses = {nonrandomCache.miss}, Number of Hits = {nonrandomCache.hits}")
            print(f"Access Time = {nonrandomCache.accessTime} nanoseconds\n")  
        print("-----------------------------------------------------------------------")
    
    #This makes the code run really smooth, since it removes the factors of associativity. 
    #Any associativity from 512+, takes 2 hours+, so This reduces it to about 2 hours
    #maxAssociativity.remove(maxAssociativity[len(maxAssociativity) - 1])