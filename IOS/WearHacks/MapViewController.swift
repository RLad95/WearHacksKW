//
//  MapViewController.swift
//  WearHacks
//
//  Created by Salman Jalali on 2016-03-18.
//  Copyright © 2016 Salman Jalali. All rights reserved.
//

import Foundation
import Mapbox
import UIKit
import CoreLocation

class MapViewController: UIViewController, MGLMapViewDelegate {
    //MARK: Properties
    var lat: Float = 43.477572
    var long: Float = -80.549226
    var name: String = "Current Location"
    var location: String = "You are currently at this location.."
    let locationManager = CLLocationManager()
    
    @IBOutlet var mapView: MGLMapView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let point = MGLPointAnnotation()
        point.coordinate = CLLocationCoordinate2D(latitude: Double(self.lat), longitude: Double(self.long))
        point.title = self.name
        point.subtitle = self.location
        
        mapView.addAnnotation(point)
        mapView.showsUserLocation = true
        mapView.zoomLevel = 14
        print("Location: \(mapView.userLocation)")
    }
    
    func mapView(mapView: MGLMapView, annotationCanShowCallout annotation: MGLAnnotation) -> Bool {
        // Always try to show a callout when an annotation is tapped.
        return true
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
}
