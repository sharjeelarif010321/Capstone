//
//  UserView.swift
//  SafeSnooze
//
//  Created by Sharjeel Arif on 2023-03-31.
//

import Foundation

import SwiftUI

struct UserView: View {
    let user: User
    
    func formattedPositionData(_ positionData: String) -> some View {
        let chunkSize = 4
        let dataSize = positionData.count
        
        // Define grid columns
        let columns: [GridItem] = Array(repeating: .init(.fixed(20)), count: 4)
        
        return LazyVGrid(columns: columns, alignment: .leading, spacing: 4) {
            ForEach(0..<(dataSize / chunkSize), id: \.self) { rowIndex in
                ForEach(0..<chunkSize, id: \.self) { colIndex in
                    let index = rowIndex * chunkSize + colIndex
                    if index < dataSize {
                        Text(String(positionData[positionData.index(positionData.startIndex, offsetBy: index)]))
                            .frame(width: 30, height: 30)
                            .background(Color.pastelBlue)
                            .border(Color.pastelDarkBlue, width: 1)
                    }
                }
            }
        }
    }


    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack {
                Text("Heart Rate:")
                    .font(.headline)
                    .foregroundColor(Color.pastelDarkBlue)
                Text("\(user.heartrate) BPM")
                    .font(.subheadline)
            }
            
            VStack(alignment: .leading, spacing: 2) {
                Text("Position:")
                    .font(.headline)
                    .foregroundColor(Color.pastelDarkBlue)
                formattedPositionData(user.position)

            }
            
            HStack {
                Text("Posture:")
                    .font(.headline)
                    .foregroundColor(Color.pastelDarkBlue)
                Text("\(user.posture)")
                    .font(.subheadline)
            }
            
            HStack {
                Text("SpO2:")
                    .font(.headline)
                    .foregroundColor(Color.pastelDarkBlue)
                Text("\(user.spo2)%")
                    .font(.subheadline)
            }
            
            HStack {
                Text("Status:")
                    .font(.headline)
                    .foregroundColor(Color.pastelDarkBlue)
                Text("\(user.status)")
                    .font(.subheadline)
            }
        }
        .frame(maxWidth: .infinity, alignment: .leading)
        .padding()
        .background(Color.pastelBlue)
        .cornerRadius(10)
        .shadow(color: Color.black.opacity(0.2), radius: 5, x: 0, y: 2)
    }
}
