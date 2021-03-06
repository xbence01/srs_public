/******************************************************************************
 * \file
 *
 * $Id:$
 *
 * Copyright (C) Brno University of Technology
 *
 * This file is part of software developed by dcgm-robotics@FIT group.
 *
 * Author: Vit Stancl (stancl@fit.vutbr.cz)
 * Supervised by: Michal Spanel (spanel@fit.vutbr.cz)
 * Date: dd/mm/2012
 * 
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this file.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#ifndef OCTOMAPPLUGIN_H_INCLUDED
#define OCTOMAPPLUGIN_H_INCLUDED

#include <srs_env_model/but_server/server_tools.h>
#include <srs_env_model/but_server/plugins/octomap_plugin_tools/testing_oriented_box.h>
#include <srs_env_model/but_server/plugins/octomap_plugin_tools/testing_sphere.h>
#include <srs_env_model/but_server/plugins/octomap_plugin_tools/testing_polymesh.h>

#include <tf/transform_listener.h>
#include <std_srvs/Empty.h>

#include <srs_env_model/RemoveCube.h>
#include <srs_env_model/AddCube.h>
#include <srs_env_model/SetCrawlingDepth.h>
#include <srs_env_model/GetTreeDepth.h>

#include <srs_env_model/LoadSave.h>

// Registration
#include <srs_env_model/but_server/registration/CPCtoOCRegistration.h>

//========================
// Filtering

#include <image_geometry/pinhole_camera_model.h>

namespace srs_env_model
{
class COctoMapPlugin : public CServerPluginBase, public CDataHolderBase< tButServerOcMap >
{
public:
	/// On new octomap data signal
	typedef boost::signal< void (SMapWithParameters &) > tSigOnNewData;

public:
	/// Constructor
	COctoMapPlugin(const std::string & name);

	/// Constructor - load data from the file
	COctoMapPlugin( const std::string & name, const std::string & filename );

	/// Destructor
	virtual ~COctoMapPlugin();

	/// Insert pointcloud
	void insertCloud( const tPointCloud& cloud);

	//! Initialize plugin - called in server constructor
	virtual void init(ros::NodeHandle & node_handle);

	//! Reset octomap
	void reset(bool clearLoaded);

	//! Get current octomap size
	unsigned getSize() { return m_data->octree.size(); }

	//! Get current tree depth
	unsigned getTreeDepth() { return m_mapParameters.treeDepth; }

	/// Get octomap resolution
	double getResolution(){ return m_mapParameters.resolution; }

	/// Crawl octomap
	void crawl( const ros::Time & currentTime );

	tSigOnNewData & getSigOnNewData() { return m_sigOnNewData; }

	//! Pause/resume plugin. All publishers and subscribers are disconnected on pause
	virtual void pause( bool bPause, ros::NodeHandle & node_handle );

protected:
	/// Should something be published?
	virtual bool shouldPublish();

	/// Publishing callback
	virtual void publishInternal(const ros::Time & timestamp);

	/// Set octomap default parameters
	void setDefaults();

	/**
	 * @brief Insert scan to the octomap
	 */
	void insertScan(const tf::Point& sensorOriginTf, const tPointCloud& ground, const tPointCloud& nonground);

	/// label the input cloud "pc" into ground and nonground. Should be in the robot's fixed frame (not world!)
	void filterGroundPlane(const tPointCloud& pc, tPointCloud& ground, tPointCloud& nonground) const;

	/// Fill map parameters
	void fillMapParameters(const ros::Time & time);

	/// Reset octomap service callback
	bool resetOctomapCB(std_srvs::Empty::Request& request,	std_srvs::Empty::Response& response);

	/// Camera info callback
	void cameraInfoCB(const sensor_msgs::CameraInfo::ConstPtr &cam_info);

	// ------------------------------------------------------------------------
	// Obstacle cleaning

	/// Remove outdated nodes
	void degradeOutdatedRaycasting(const std_msgs::Header& sensor_header, const octomap::point3d& sensor_origin);

	/// Remove speckles
	void degradeSingleSpeckles();

	/// Compute bounding box from the sensor position and cone
	void computeBBX(const std_msgs::Header& sensor_header, octomap::point3d& bbx_min, octomap::point3d& bbx_max);

	/// Is point in sensor cone?
	bool inSensorCone(const cv::Point2d& uv) const;

	/// Return true, if occupied cell is between origin and p
	bool isOccludedMap(const octomap::point3d& sensor_origin, const octomap::point3d& p) const;

	/// Get used sensor origin
	octomap::point3d getSensorOrigin(const std_msgs::Header& sensor_header);

	/// Do octomap testing by object
	long int doObjectTesting( CTestingObjectBase * object );

	/// Remove cube as a service - callback
	bool removeCubeCB( srs_env_model::RemoveCube::Request & req, srs_env_model::RemoveCube::Response & res );

	/// Remove cube as a service - callback
	bool addCubeCB( srs_env_model::AddCube::Request & req, srs_env_model::AddCube::Response & res );

	/// For debugging purpouses - add cubical interactive marker to the scene
	void addCubeGizmo( const geometry_msgs::Pose & pose, const geometry_msgs::Point & size );

	/// Set crawling depth - service callback
	bool setCrawlingDepthCB( srs_env_model::SetCrawlingDepth::Request & req, srs_env_model::SetCrawlingDepth::Response & res );

	/// Get octomap tree depth - service callback
	bool getTreeDepthCB( srs_env_model::GetTreeDepth::Request & req, srs_env_model::GetTreeDepth::Response & res );

	/// Load map service callback
	bool loadOctreeCB( srs_env_model::LoadSaveRequest & req, srs_env_model::LoadSaveResponse & res );

	/// Save map service callback
	bool saveOctreeCB( srs_env_model::LoadSaveRequest & req, srs_env_model::LoadSaveResponse & res );

protected:

    /// Should ground plane be filtered?
    bool m_filterGroundPlane;
    double m_groundFilterDistance;
    double m_groundFilterAngle;
    double m_groundFilterPlaneDistance;

    /// Temporary storage for ray casting
    octomap::KeyRay m_keyRay;

    /// On traversal start
    tSigOnNewData m_sigOnNewData;

     /// Octomap parameters
    SMapWithParameters m_mapParameters;

    //! Transform listener
    tf::TransformListener m_tfListener;

    /// Reset octomap service
    ros::ServiceServer m_serviceResetOctomap;

    /// Remove oriented box from octomap service
    ros::ServiceServer m_serviceRemoveCube;

    /// Add oriented box to the octomap service
    ros::ServiceServer m_serviceAddCube;

    /// Set crawling depth
    ros::ServiceServer m_serviceSetCrawlDepth;

    /// Get octtree depth service
    ros::ServiceServer m_serviceGetTreeDepth;

    /// Load map service
    ros::ServiceServer m_serviceLoadMap;

    /// Save map service
    ros::ServiceServer m_serviceSaveMap;

    /// Should octomap be published
    bool m_bPublishOctomap;

    //! Octomap publisher name
    std::string m_ocPublisherName;

    /// Octomap publisher
    ros::Publisher m_ocPublisher;

    bool m_latchedTopics;

    /// Remove specle nodes now
    bool m_removeSpecles;

    int filecounter;

    //=========================================================================
    // Filtering

    /// Should be outdated nodes be removed?
    bool m_bRemoveOutdated;

    /// Camera model
    image_geometry::PinholeCameraModel m_camera_model;

    /// Is camera model initialized?
    bool m_bCamModelInitialized;

    /// Camera size
    cv::Size m_camera_size;

    /// Camera info topic name
    std::string m_camera_info_topic;

    //! Camera frame id
    std::string m_camFrameId;

    /// Camera info subscriber
    ros::Subscriber m_ciSubscriber;

    /// Should be markers visualized
    bool m_bVisualizeMarkers;

    /// Markers visualizing publisher
    ros::Publisher m_markerPublisher;

    /// Markers topic name
    std::string m_markers_topic_name;

    /// Camera offsets
    int m_camera_stereo_offset_left, m_camera_stereo_offset_right;

    /// Filtering object
    CTestingObjectBase * m_removeTester;

    /// Tester life in scans count
    unsigned int m_testerLife;

    /// Tester life counter
    unsigned int m_testerLifeCounter;

    /// Camera info locking
    boost::mutex m_lockCamera;

    /// Maximal depth of tree used when crawling
    unsigned char m_crawlDepth;

    //! Registration module
    CPcToOcRegistration m_registration;

    //! First frame is already inserted
    bool m_bNotFirst;

    //! Was map loaded
    bool m_bMapLoaded;

}; // class COctoMapPlugin;


}



 // namespace srs_env_model

// OCTOMAPPLUGIN_H_INCLUDED
#endif

