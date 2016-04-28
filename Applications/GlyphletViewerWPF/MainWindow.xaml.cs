//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using GlyphCLR;
//--------------------------------------------------------------------------------
namespace GlyphletViewerWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private D3DImageEx m_d3DImageEx;
        private ManagedGlyphlet m_d3D11Scene;

        public MainWindow()
        {
            InitializeComponent();
            Loaded += MainWindow_Loaded;
            Closing += MainWindow_Closing;
        }


        void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            /* Create a new D3DImageEx class */
            m_d3DImageEx = new D3DImageEx();

            /* Set our image's source to our D3DImage9Ex */
            d3dScene.Source = m_d3DImageEx;

            /* Create a new D3DTest scene */
            m_d3D11Scene = new ManagedGlyphlet();
            m_d3D11Scene.Initialize();

            /* Set the backbuffer, which is a ID3D10Texture2D pointer */
            m_d3DImageEx.SetBackBufferEx(m_d3D11Scene.GetRenderTarget());

            CompositionTarget.Rendering += CompositionTarget_Rendering;
        }

        void MainWindow_Closing(object sender, object e)
        {
            CompositionTarget.Rendering -= CompositionTarget_Rendering;
            m_d3D11Scene.Shutdown();
        }

        /// <summary>
        /// Invalidates entire D3DImage area
        /// </summary>
        private void InvalidateD3DImage()
        {
            m_d3DImageEx.Lock();
            m_d3DImageEx.AddDirtyRect(new Int32Rect()
            {
                X = 0,
                Y = 0,
                Height = m_d3DImageEx.PixelHeight,
                Width = m_d3DImageEx.PixelWidth
            });
            m_d3DImageEx.Unlock();

        }

        private void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            /* Render D3D10 test scene */
            m_d3D11Scene.Update(0.01f);

            /* Invalidate our D3DImage */
            InvalidateD3DImage();
        }

    }
}
//--------------------------------------------------------------------------------