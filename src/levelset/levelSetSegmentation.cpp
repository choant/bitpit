/*---------------------------------------------------------------------------*\
 *
 *  bitpit
 *
 *  Copyright (C) 2015-2017 OPTIMAD engineering Srl
 *
 *  -------------------------------------------------------------------------
 *  License
 *  This file is part of bitpit.
 *
 *  bitpit is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License v3 (LGPL)
 *  as published by the Free Software Foundation.
 *
 *  bitpit is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with bitpit. If not, see <http://www.gnu.org/licenses/>.
 *
\*---------------------------------------------------------------------------*/

# include <cassert>

# include "bitpit_common.hpp"
# include "bitpit_operators.hpp"
# include "bitpit_CG.hpp"
# include "bitpit_surfunstructured.hpp"
# include "bitpit_volcartesian.hpp"

# include "levelSetKernel.hpp"
# include "levelSetCartesian.hpp"
# include "levelSetOctree.hpp"

# include "levelSetObject.hpp"
# include "levelSetCachedObject.hpp"
# include "levelSetSegmentation.hpp"

namespace bitpit {

/*!
    @class      SegmentationKernel
    @ingroup    levelset
    @brief      Segmentation kernel
*/

/*!
 * Default constructor
 */
SegmentationKernel::SegmentationKernel( ) : m_surface(nullptr), m_featureAngle(0) {
}

/*!
 * Constructor
 */
SegmentationKernel::SegmentationKernel( std::unique_ptr<const SurfUnstructured> &&surface, double featureAngle ) {

    m_ownedSurface = std::shared_ptr<const SurfUnstructured>(surface.release());

    setSurface(m_ownedSurface.get(), featureAngle);
}

/*!
 * Constructor
 */
SegmentationKernel::SegmentationKernel( const SurfUnstructured *surface, double featureAngle ) {

    setSurface(surface, featureAngle);
}

/*!
 * Get feature angle
 * @return feature angle used when calculating face normals;
 */
double SegmentationKernel::getFeatureAngle() const {
    return m_featureAngle;
}

/*!
 * Get segmentation vertex normals
 * @return segmentation vertex normals;
 */
const std::unordered_map<long, std::vector< std::array<double,3>>> & SegmentationKernel::getVertexNormals() const {
    return m_vertexNormals;
}

/*!
 * Get segmentation vertex gradients
 * @return segmentation vertex gradients;
 */
const std::unordered_map<long, std::vector< std::array<double,3>>> & SegmentationKernel::getVertexGradients() const {
    return m_vertexGradients;
}

/*!
 * Get segmentation surface
 * @return segmentation surface;
 */
const SurfUnstructured & SegmentationKernel::getSurface() const {
    return *m_surface;
}

/*!
 * Set the surface
 * @param[in] patch pointer to surface
 */
void SegmentationKernel::setSurface( const SurfUnstructured *surface, double featureAngle){

    std::vector<std::array<double,3>> vertexNormal ;
    std::vector<std::array<double,3>> vertexGradient ;

    m_surface      = surface;
    m_featureAngle = featureAngle;

    double tol = m_surface->getTol() ;
    for( const Cell &segment : m_surface->getCells() ){
        long segmentId = segment.getId() ;
        int nVertices  = segment.getVertexCount() ;

        vertexNormal.resize(nVertices) ;
        vertexGradient.resize(nVertices) ;

        double misalignment = 0. ;
        for( int i = 0; i < nVertices; ++i ){
            vertexGradient[i] = m_surface->evalVertexNormal(segmentId, i) ;
            vertexNormal[i]   = m_surface->evalLimitedVertexNormal(segmentId, i, m_featureAngle) ;

            misalignment += norm2(vertexGradient[i] - vertexNormal[i]) ;
        }

        m_vertexGradients.insert({{segmentId, vertexGradient}}) ;
        if( misalignment >= tol ){
            m_vertexNormals.insert({{segmentId, vertexNormal}}) ;
        }
    }

    // Initialize search tree
    m_searchTreeUPtr = std::unique_ptr<SurfaceSkdTree>(new SurfaceSkdTree(surface));
    m_searchTreeUPtr->build();
}

/*!
 * Get the coordinates of the specified segment's vertices.
 * @param[in] id segmment's id
 * @param[out] coords on output will contain coordinates of the vertices
 */
void SegmentationKernel::getSegmentVertexCoords( long id, std::vector<std::array<double,3>> *coords ) const {

    const Cell &segment = m_surface->getCell(id) ;
    int nVertices = segment.getVertexCount() ;

    coords->resize(nVertices);
    for (int n = 0; n < nVertices; ++n) {
        long vertexId = segment.getVertex(n) ;
        (*coords)[n] = m_surface->getVertexCoords(vertexId);
    }

    if ( nVertices > 3 ) {
        log::cout() << "levelset: only segments and triangles supported in LevelSetSegmentation !!" << std::endl ;
    }
}

/*!
 * Computes levelset relevant information at one point with respect to a segment
 * @param[in] p coordinates of point
 * @param[in] i index of segment
 * @param[out] d distance point to segment
 * @param[out] s sign of point wrt to segment, i.e. according to normal.
 * Care should be taken since the method could return erroneous information when
 * the point p lies on the normal plane. In this case s=0 but due to surface curvature
 * the point may not lie necessary on the surface. This sititaion is easily indentified
 * because the distance != 0.
 * @param[out] x closest point on segment
 * @param[out] n normal at closest point
 */
void SegmentationKernel::getSegmentInfo( const std::array<double,3> &p, const long &i, const bool &signd, double &d, std::array<double,3> &g, std::array<double,3> &n ) const {

    std::array<double,3> x ;

    auto itrNormal = getVertexNormals().find(i) ;
    auto itrGradient = getVertexGradients().find(i) ;
    assert( itrGradient != getVertexGradients().end() ) ;

    const Cell &cell = m_surface->getCell(i) ;
    int nVertices = cell.getVertexCount() ;
    switch (nVertices) {

    case 1:
    {
        long id = cell.getVertex(0) ;
        x = m_surface->getVertexCoords(id);
        g = p-x;
        d = norm2(g);
        g /= d;

        n = g;

        break;
    }

    case 2:
    {
        long id0 = cell.getVertex(0) ;
        long id1 = cell.getVertex(1) ;

        std::array<double,2> lambda ;
        int flag ;

        d= CGElem::distancePointSegment( p, m_surface->getVertexCoords(id0), m_surface->getVertexCoords(id1), x, lambda, flag ) ;

        g = p-x;
        g /= norm2(g);

        n  = lambda[0] *itrGradient->second[0] ;
        n += lambda[1] *itrGradient->second[1] ;
        n /= norm2(n) ;

        g *= sign(dotProduct(g,n));

        if( itrNormal != getVertexNormals().end() ){
            n  = lambda[0] *itrNormal->second[0] ;
            n += lambda[1] *itrNormal->second[1] ;
            n /= norm2(n) ;

            double kappa ;
            maxval(lambda,kappa);
            kappa = 1. -kappa;

            n *= kappa;
            n += (1.-kappa)*g;
            n /= norm2(n);

        }

        break;
    }

    case 3:
    {
        long id0 = cell.getVertex(0) ;
        long id1 = cell.getVertex(1) ;
        long id2 = cell.getVertex(2) ;

        std::array<double,3> lambda ;
        int flag ;

        d= CGElem::distancePointTriangle( p, m_surface->getVertexCoords(id0), m_surface->getVertexCoords(id1), m_surface->getVertexCoords(id2), x, lambda, flag ) ;

        g = p-x;
        g /= norm2(g);

        n  = lambda[0] *itrGradient->second[0] ;
        n += lambda[1] *itrGradient->second[1] ;
        n += lambda[2] *itrGradient->second[2] ;
        n /= norm2(n);

        g *= sign(dotProduct(g,n));

        if( itrNormal != getVertexNormals().end() ){
            n  = lambda[0] *itrNormal->second[0] ;
            n += lambda[1] *itrNormal->second[1] ;
            n += lambda[2] *itrNormal->second[2] ;
            n /= norm2(n) ;

            double kappa ;
            maxval(lambda,kappa);
            kappa = 1. -kappa;

            n *= kappa;
            n += (1.-kappa)*g;
            n /= norm2(n);

        }

        break;
    }

    default:
    {
        log::cout() << " Segment not supported in SegmentationKernel::getSegmentInfo " << nVertices << std::endl ;

        break;
    }

    }

    double s = sign( dotProduct(g, p - x) );

    d *= ( signd *s  + (!signd) *1.);
    n *= ( signd *1. + (!signd) *s ) ;

    return;

}

/*!
	@class      LevelSetSegmentation
	@ingroup    levelset
	@brief      Implements visitor pattern fo segmentated geometries
*/

/*!
 * Destructor
 */
LevelSetSegmentation::~LevelSetSegmentation() {
}

/*!
 * Constructor
 * @param[in] id identifier of object
 */
LevelSetSegmentation::LevelSetSegmentation(int id) : LevelSetCachedObject(id), m_segmentation(nullptr) {
}

/*!
 * Constructor
 * @param[in] id identifier of object
 * @param[in] STL unique pointer to surface mesh
 * @param[in] featureAngle feature angle; if the angle between two segments is bigger than this angle, the enclosed edge is considered as a sharp edge.
 */
LevelSetSegmentation::LevelSetSegmentation( int id, std::unique_ptr<const SurfUnstructured> &&STL, double featureAngle) :LevelSetSegmentation(id) {
    setSegmentation( std::move(STL), featureAngle );
}

/*!
 * Constructor
 * @param[in] id identifier of object
 * @param[in] STL pointer to surface mesh
 * @param[in] featureAngle feature angle; if the angle between two segments is bigger than this angle, the enclosed edge is considered as a sharp edge.
 */
LevelSetSegmentation::LevelSetSegmentation( int id, const SurfUnstructured *STL, double featureAngle) :LevelSetSegmentation(id) {
    setSegmentation( STL, featureAngle );
}

/*!
 * Clones the object
 * @return pointer to cloned object
 */
LevelSetSegmentation* LevelSetSegmentation::clone() const {
    return new LevelSetSegmentation( *this ); 
}

/*!
 * Set the segmentation
 * @param[in] patch pointer to surface
 */
void LevelSetSegmentation::setSegmentation( const SurfUnstructured *surface, double featureAngle){

    m_segmentation = std::make_shared<const SegmentationKernel>(surface, featureAngle);
}

/*!
 * Set the segmentation
 * @param[in] patch pointer to surface
 */
void LevelSetSegmentation::setSegmentation( std::unique_ptr<const SurfUnstructured> &&surface, double featureAngle){

    m_segmentation = std::make_shared<const SegmentationKernel>(std::move(surface), featureAngle);
}

/*!
 * Get a constant refernce to the segmentation
 * @return constant reference to the segmentation
 */
const SegmentationKernel & LevelSetSegmentation::getSegmentation() const {
    return *m_segmentation ;
}

/*!
 * Gets the closest support within the narrow band of cell
 * @param[in] id index of cell
 * @return closest segment in narrow band
 */
int LevelSetSegmentation::getPart( const long &id ) const{

    long supportId = getSupport(id);

    if( supportId != levelSetDefaults::SUPPORT){
        const SurfUnstructured &m_surface = m_segmentation->getSurface();
        return m_surface.getCell(supportId).getPID();
    } else { 
        return levelSetDefaults::PART ;
    }

}

/*!
 * Gets the closest support within the narrow band of cell
 * @param[in] id index of cell
 * @return closest segment in narrow band
 */
long LevelSetSegmentation::getSupport( const long &id ) const{

    auto itr = m_support.find(id) ;
    if( itr != m_support.end() ){
        return *itr;
    } else {
        return levelSetDefaults::SUPPORT ;
    }

}

/*!
 * Get size of support triangle
 * @param[in] i cell index
 * @return charcteristic size of support triangle
 */
double LevelSetSegmentation::getSurfaceFeatureSize( const long &i ) const {

    long support = getSupport(i);
    if (support == levelSetDefaults::SUPPORT) {
        return (- levelSetDefaults::SIZE);
    }

    return getSegmentSize(support);
}

/*!
 * Get the sie of a segment
 * @param[in] id is the id of the segment
 * @return charcteristic size of the segment
 */
double LevelSetSegmentation::getSegmentSize( long id ) const {

    const SurfUnstructured &m_surface = m_segmentation->getSurface();

    int spaceDimension = m_surface.getSpaceDimension();
    if (spaceDimension == 2) {
        return m_surface.evalCellArea(id); //TODO check
    } else if (spaceDimension == 3) {
        int dummy;
        return m_surface.evalMinEdgeLength(id, dummy);
    }

    return (- levelSetDefaults::SIZE);
}

/*!
 * Get the smallest characterstic size within the triangultaion
 * @return smallest charcteristic size within the triangulation
 */
double LevelSetSegmentation::getMinSurfaceFeatureSize( ) const {

    const SurfUnstructured &m_surface = m_segmentation->getSurface();

    bool   minimumValid = false;
    double minimumSize  = levelSetDefaults::SIZE;
    for( const Cell &cell : m_surface.getCells() ){
        double segmentSize = getSegmentSize(cell.getId());
        if (segmentSize < 0) {
            continue;
        }

        minimumValid = true;
        minimumSize  = std::min(segmentSize, minimumSize);
    }

    if (!minimumValid) {
        minimumSize = - levelSetDefaults::SIZE;
    }

    return minimumSize;
}

/*!
 * Get the largest characterstic size within the triangultaion
 * @return largest charcteristic size within the triangulation
 */
double LevelSetSegmentation::getMaxSurfaceFeatureSize( ) const {

    const SurfUnstructured &m_surface = m_segmentation->getSurface();

    double maximumSize = - levelSetDefaults::SIZE;
    for( const Cell &cell : m_surface.getCells() ){
        double segmentSize = getSegmentSize(cell.getId());
        maximumSize = std::max(segmentSize, maximumSize);
    }

    return maximumSize;
}


/*!
 * Finds seed points in narrow band within a cartesian mesh for one simplex
 * @param[in] visitee cartesian mesh 
 * @param[in] VS Simplex
 * @param[out] I indices of seed points
 */
bool LevelSetSegmentation::seedNarrowBand( LevelSetCartesian *visitee, std::vector<std::array<double,3>> &VS, std::vector<long> &I){

    VolCartesian                        &mesh = *(static_cast<VolCartesian*>(visitee->getMesh()));

    bool                                found(false) ;
    int                                 dim( mesh.getDimension() ) ;
    std::array<double,3>                B0, B1;
    std::vector<std::array<double,3>>   VP ;

    mesh.getBoundingBox(B0, B1) ;

    for( int i=0; i<dim; ++i){
        B0[i] -= getSizeNarrowBand() ;
        B1[i] += getSizeNarrowBand() ;
    }

    I.clear() ;

    for( const auto &P : VS){
        if(  CGElem::intersectPointBox( P, B0, B1, dim ) ) {
            I.push_back( mesh.locateClosestCell(P) );
            found =  true ;
        }
    }

    if( !found && CGElem::intersectBoxSimplex( B0, B1, VS, VP, dim ) ) {
        for( const auto &P : VP){
            I.push_back( mesh.locateClosestCell(P) );
            found = true ;
        }
    }

    return found ;
}

/*!
 * Computes axis aligned bounding box of object
 * @param[out] minP minimum point
 * @param[out] maxP maximum point
 */
void LevelSetSegmentation::getBoundingBox( std::array<double,3> &minP, std::array<double,3> &maxP ) const {
    const SurfUnstructured &m_surface = m_segmentation->getSurface();
    m_surface.getBoundingBox(minP,maxP) ;
}

/*!
 * Clear the segmentation and the specified kernel.
 */
void LevelSetSegmentation::__clear( ){

    m_support.clear() ;
}

/*!
 * Computes the levelset function within the narrow band
 * @param[in] RSearch size of narrow band
 * @param[in] signd if signed- or unsigned- distance function should be calculated
 */
void LevelSetSegmentation::computeLSInNarrowBand( const double &RSearch, const bool &signd ){

    log::cout() << "Computing levelset within the narrow band... " << std::endl;

    if( LevelSetCartesian* lsCartesian = dynamic_cast<LevelSetCartesian*>(m_kernelPtr) ){
        computeLSInNarrowBand( lsCartesian, RSearch, signd ) ;

    } else if ( LevelSetOctree* lsOctree = dynamic_cast<LevelSetOctree*>(m_kernelPtr) ){
        computeLSInNarrowBand( lsOctree, RSearch, signd ) ;

    }
}

/*!
 * Updates the levelset function within the narrow band after mesh adaptation.
 * @param[in] mapper information concerning mesh adaption 
 * @param[in] RSearch size of narrow band
 * @param[in] signd if signed- or unsigned- distance function should be calculated
 */
void LevelSetSegmentation::updateLSInNarrowBand( const std::vector<adaption::Info> &mapper, const double &RSearch, const bool &signd ){

    // Update is not implemented for Cartesian patches
    if( dynamic_cast<LevelSetCartesian*>(m_kernelPtr) ){
        clear( ) ;
        computeLSInNarrowBand( RSearch, signd ) ;
        return;
    }

    if( LevelSetOctree* lsOctree = dynamic_cast<LevelSetOctree*>(m_kernelPtr) ){
        log::cout() << "Updating levelset within the narrow band... " << std::endl;
        updateLSInNarrowBand( lsOctree, mapper, RSearch, signd ) ;
        return;
    }


}

/*!
 */
void LevelSetSegmentation::computeLSInNarrowBand( LevelSetCartesian *visitee, const double &RSearch, const bool &signd ){

    VolumeKernel                            &mesh = *(visitee->getMesh() ) ;
    std::vector<std::array<double,3>>       VS(3);

    const SurfUnstructured                  &m_surface = m_segmentation->getSurface();

    std::vector<long>                       stack, temp, neighs, flag( mesh.getCellCount(), -1);

    std::vector< std::array<double,3> >     cloud ;
    std::vector<double>                     cloudDistance;

    double distance;
    std::array<double,3>  gradient, normal;

    stack.reserve(128) ;
    temp.reserve(128) ;

    log::cout() << " Compute levelset on cartesian mesh"  << std::endl;

    for (const Cell &segment : m_surface.getCells()) {
        long segmentId = segment.getId();

        // compute initial seeds, ie the cells where the vertices
        // of the surface element fall in and add them to stack
        m_segmentation->getSegmentVertexCoords( segmentId, &VS ) ;
        seedNarrowBand( visitee, VS, stack );


        // propagate from seed
        size_t stackSize = stack.size();
        while (stackSize > 0) {

            // put the cell centroids of the stack into a vector
            // and calculate the distances to the cloud
            cloud.resize(stackSize) ;

            for( size_t k = 0; k < stackSize; ++k) {
                long cell = stack[k];
                cloud[k] = visitee->computeCellCentroid(cell) ;
            }
            cloudDistance = CGElem::distanceCloudSimplex( cloud, VS); 

            // check each cell of cloud individually
            for( size_t k = 0; k < stackSize; ++k) {

                long &cellId = stack[k];
                double &cellDistance = cloudDistance[k];

                // consider only cells within the search radius
                if ( cellDistance <= RSearch ) {

                    PiercedVector<LevelSetInfo>::iterator lsInfoItr = m_ls.find(cellId) ;
                    if( lsInfoItr == m_ls.end() ){
                        lsInfoItr = m_ls.emplace(cellId) ;
                    }


                    // check if the computed distance is the closest distance
                    if( cellDistance < std::abs(lsInfoItr->value) ){

                        // compute all necessary information and store them
                        m_segmentation->getSegmentInfo(cloud[k], segmentId, signd, distance, gradient, normal);

                        lsInfoItr->value    = distance;
                        lsInfoItr->gradient = normal;
                
                        PiercedVector<long>::iterator supportItr = m_support.find(cellId) ;
                        if( supportItr == m_support.end() ){
                            supportItr = m_support.emplace(cellId) ;
                        }

                        *supportItr = segmentId;
                    }


                    // the new stack is composed of all neighbours
                    // of the old stack. Attention must be paid in 
                    // order not to evaluate the same cell twice
                    neighs.clear();
                    mesh.findCellFaceNeighs(cellId, &neighs) ;
                    for( const auto &  neigh : neighs){
                        if( flag[neigh] != segmentId) {
                            temp.push_back( neigh) ;
                            flag[neigh] = segmentId ;
                        }
                    }

                } //end if distance

            }

            stack.clear() ;
            stack.swap( temp ) ;
            stackSize = stack.size() ;


        } //end while


    }

    return;

}

/*!
 */
void LevelSetSegmentation::computeLSInNarrowBand( LevelSetOctree *visitee, const double &RSearch, const bool &signd){


    VolumeKernel &mesh = *(visitee->getMesh()) ;

    bool adaptiveSearch(RSearch<0);
    double searchRadius = RSearch;

    long segmentId;
    double distance;
    std::array<double,3> gradient, normal;

    for( const Cell &cell : mesh.getCells() ){
        long cellId = cell.getId();
        std::array<double, 3> cellCentroid = visitee->computeCellCentroid(cellId);

        if(adaptiveSearch){
            double cellSize = mesh.evalCellSize(cellId);
            searchRadius = 0.5*sqrt(3.)*cellSize;
        }

        m_segmentation->m_searchTreeUPtr->findPointClosestCell(cellCentroid, searchRadius, &segmentId, &distance);

        if(segmentId>=0){

            m_segmentation->getSegmentInfo(cellCentroid, segmentId, signd, distance, gradient, normal);

            PiercedVector<LevelSetInfo>::iterator lsInfoItr = m_ls.emplace(cellId) ;
            lsInfoItr->value    = distance;
            lsInfoItr->gradient = normal;

            PiercedVector<long>::iterator supportItr = m_support.emplace(cellId) ;
            *supportItr = segmentId;

        }
    }
}

/*!
 */
void LevelSetSegmentation::updateLSInNarrowBand( LevelSetOctree *visitee, const std::vector<adaption::Info> &mapper, const double &RSearch, const bool &signd){

    clearAfterMeshAdaption(mapper);

    VolumeKernel &mesh = *(visitee->getMesh()) ;

    bool adaptiveSearch(RSearch<0);
    double searchRadius = RSearch;

    long segmentId;
    std::array<double,3> cellCentroid;

    double distance;
    std::array<double,3> gradient, normal;

    for( const auto &event : mapper ){

        if( event.entity != adaption::Entity::ENTITY_CELL ){
            continue;
        }

        for( const long &cellId : event.current ){
            cellCentroid = visitee->computeCellCentroid(cellId);

            if(adaptiveSearch){
                double cellSize = mesh.evalCellSize(cellId);
                searchRadius = 0.5*sqrt(3.)*cellSize;
            }

            m_segmentation->m_searchTreeUPtr->findPointClosestCell(cellCentroid, searchRadius, &segmentId, &distance);

            if(segmentId>=0){

                m_segmentation->getSegmentInfo(cellCentroid, segmentId, signd, distance, gradient, normal);

                PiercedVector<LevelSetInfo>::iterator lsInfoItr = m_ls.emplace(cellId) ;
                lsInfoItr->value    = distance;
                lsInfoItr->gradient = normal;

                PiercedVector<long>::iterator supportItr = m_support.emplace(cellId) ;
                *supportItr = segmentId;

            }
        }
    }
}

/*!
 * Prune the segment's info removing entries associated to cells that are
 * are not in the mesh anymore
 * @param[in] mapper information concerning mesh adaption
 */
void LevelSetSegmentation::__clearAfterMeshAdaption( const std::vector<adaption::Info> &mapper ){

    for ( const auto &info : mapper ){
        // Consider only changes on cells
        if( info.entity != adaption::Entity::ENTITY_CELL ){
            continue;
        }

        // Delete only old data that belongs to the current processor
        if (info.type == adaption::Type::TYPE_PARTITION_RECV) {
            continue;
        }

        // Remove info of previous cells
        for ( const long & parent : info.previous ) {
            if ( m_support.find( parent ) == m_support.end() ) {
                continue;
            }

            m_support.erase( parent, true ) ;
        }
    }

    m_support.flush();
}

/*!
 * Clears data structure outside narrow band
 * @param[in] search size of narrow band
 */
void LevelSetSegmentation::__filterOutsideNarrowBand( double search ){

    long id ;

    bitpit::PiercedVector<long>::iterator supportItr ;
    for( supportItr = m_support.begin(); supportItr != m_support.end(); ++supportItr){
        id = supportItr.getId() ;
        if( ! isInNarrowBand(id) ){
            m_support.erase(id,true) ;
        }
    }

    m_support.flush() ;

}

/*!
 * Writes LevelSetSegmentation to stream in binary format
 * @param[in] stream output stream
 */
void LevelSetSegmentation::__dump( std::ostream &stream ){

    utils::binary::write( stream, m_support.size() ) ;

    bitpit::PiercedVector<long>::iterator supportItr, supportEnd = m_support.end() ;
    for( supportItr = m_support.begin(); supportItr != supportEnd; ++supportItr){
        utils::binary::write( stream, supportItr.getId() );
        utils::binary::write( stream, *supportItr );
    }
}

/*!
 * Reads LevelSetSegmentation from stream in binary format
 * @param[in] stream output stream
 */
void LevelSetSegmentation::__restore( std::istream &stream ){

    size_t supportSize;
    utils::binary::read( stream, supportSize ) ;
    m_support.reserve(supportSize);

    for( size_t i=0; i<supportSize; ++i){
        long id;
        utils::binary::read( stream, id );

        long support;
        utils::binary::read( stream, support );

        m_support.insert(id,support) ;
    }
}

# if BITPIT_ENABLE_MPI

/*!
 * Flushing of data to communication buffers for partitioning
 * @param[in] sendList list of cells to be sent
 * @param[in,out] dataBuffer buffer for second communication containing data
 */
void LevelSetSegmentation::__writeCommunicationBuffer( const std::vector<long> &sendList, SendBuffer &dataBuffer ){

    long nItems(0);

    //determine number of elements to send
    for( const auto &index : sendList){
        auto supportItr = m_support.find(index) ;
        if( supportItr != m_support.end() ){
            nItems++ ;
        }
    }

    dataBuffer << nItems ;
    dataBuffer.setSize(dataBuffer.getSize() +nItems* sizeof(long) );

    //determine elements to send
    long counter= 0 ;
    for( const auto &index : sendList){
        auto supportItr = m_support.find(index) ;
        if( supportItr != m_support.end() ){
            dataBuffer << counter ;
            dataBuffer << *supportItr;
        }

        ++counter;
    }
}

/*!
 * Processing of communication buffer into data structure
 * @param[in] recvList list of cells to be received
 * @param[in,out] dataBuffer buffer containing the data
 */
void LevelSetSegmentation::__readCommunicationBuffer( const std::vector<long> &recvList, RecvBuffer &dataBuffer ){

    long    nItems, index, id ;

    dataBuffer >> nItems ;

    for( int i=0; i<nItems; ++i){

        // Determine the id of the element
        dataBuffer >> index ;
        id = recvList[index] ;

        // Assign the data of the element
        PiercedVector<long>::iterator supportItr = m_support.find(id) ;
        if( supportItr == m_support.end() ){
            supportItr = m_support.emplace(id) ;
        }

        dataBuffer >> *supportItr ;
    }
}
# endif

}
